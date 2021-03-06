/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "Catalog.hxx"
#include "Connection.hxx"
#include "DatabaseMetaData.hxx"
#include "Driver.hxx"
#include "PreparedStatement.hxx"
#include "Statement.hxx"
#include "Tables.hxx"
#include "Util.hxx"

#include <stdexcept>

#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFrames.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/io/TempFile.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/TransactionIsolation.hpp>
#include <com/sun/star/ucb/SimpleFileAccess.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess2.hpp>

#include <connectivity/dbexception.hxx>
#include <connectivity/sqlparse.hxx>
#include <resource/common_res.hrc>
#include <resource/hsqldb_res.hrc>
#include <resource/sharedresources.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/storagehelper.hxx>
#include <unotools/tempfile.hxx>
#include <unotools/localfilehelper.hxx>
#include <unotools/ucbstreamhelper.hxx>

using namespace connectivity::firebird;
using namespace connectivity;

using namespace ::osl;

using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::embed;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::uno;

const OUString Connection::our_sFBKLocation( "firebird.fbk" );

Connection::Connection(FirebirdDriver*    _pDriver)
    : Connection_BASE(m_aMutex)
    , OSubComponent<Connection, Connection_BASE>(static_cast<cppu::OWeakObject*>(_pDriver), this)
    , m_xDriver(_pDriver)
    , m_sConnectionURL()
    , m_sFirebirdURL()
    , m_bIsEmbedded(false)
    , m_xEmbeddedStorage(0)
    , m_bIsFile(false)
    , m_sUser()
    , m_bIsAutoCommit(false)
    , m_bIsReadOnly(false)
    , m_aTransactionIsolation(TransactionIsolation::REPEATABLE_READ)
    , m_aDBHandle(0)
    , m_aTransactionHandle(0)
    , m_xCatalog(0)
    , m_xMetaData(0)
    , m_aStatements()
{
}

Connection::~Connection()
{
    if(!isClosed())
        close();
}

void SAL_CALL Connection::release() throw()
{
    relase_ChildImpl();
}

struct ConnectionGuard
{
    oslInterlockedCount& m_refCount;
    explicit ConnectionGuard(oslInterlockedCount& refCount)
        : m_refCount(refCount)
    {
        osl_atomic_increment(&m_refCount);
    }
    ~ConnectionGuard()
    {
        osl_atomic_decrement(&m_refCount);
    }
};

void Connection::construct(const ::rtl::OUString& url, const Sequence< PropertyValue >& info)
    throw (SQLException, RuntimeException, std::exception)
{
    ConnectionGuard aGuard(m_refCount);

    try
    {
        m_sConnectionURL = url;

        bool bIsNewDatabase = false;
        OUString aStorageURL;
        if (url == "sdbc:embedded:firebird")
        {
            m_bIsEmbedded = true;

            const PropertyValue* pIter = info.getConstArray();
            const PropertyValue* pEnd = pIter + info.getLength();

            for (;pIter != pEnd; ++pIter)
            {
                if ( pIter->Name == "Storage" )
                {
                    m_xEmbeddedStorage.set(pIter->Value,UNO_QUERY);
                }
                else if ( pIter->Name == "URL" )
                {
                    pIter->Value >>= aStorageURL;
                }
                else if ( pIter->Name == "Document" )
                {
                    pIter->Value >>= m_xParentDocument;
                }
            }

            if ( !m_xEmbeddedStorage.is() )
            {
                ::connectivity::SharedResources aResources;
                const OUString sMessage = aResources.getResourceString(STR_NO_STORAGE);
                ::dbtools::throwGenericSQLException(sMessage ,*this);
            }

            bIsNewDatabase = !m_xEmbeddedStorage->hasElements();

            m_pDatabaseFileDir.reset(new ::utl::TempFile(NULL, true));
            m_sFirebirdURL = m_pDatabaseFileDir->GetFileName() + "/firebird.fdb";
            m_sFBKPath = m_pDatabaseFileDir->GetFileName() + "/firebird.fbk";

            SAL_INFO("connectivity.firebird", "Temporary .fdb location:  " << m_sFirebirdURL);

            if (!bIsNewDatabase)
            {
                SAL_INFO("connectivity.firebird", "Extracting .fbk from .odb" );
                 if (!m_xEmbeddedStorage->isStreamElement(our_sFBKLocation))
                {
                    ::connectivity::SharedResources aResources;
                    const OUString sMessage = aResources.getResourceString(STR_ERROR_NEW_VERSION);
                    ::dbtools::throwGenericSQLException(sMessage ,*this);
                }

                 Reference< XStream > xDBStream(m_xEmbeddedStorage->openStreamElement(our_sFBKLocation,
                                                                            ElementModes::READ));

                uno::Reference< ucb::XSimpleFileAccess2 > xFileAccess(
                        ucb::SimpleFileAccess::create( comphelper::getProcessComponentContext() ),
                                                                    uno::UNO_QUERY );
                if ( !xFileAccess.is() )
                {
                    ::connectivity::SharedResources aResources;
                    const OUString sMessage = aResources.getResourceString(STR_ERROR_NEW_VERSION);
                    ::dbtools::throwGenericSQLException(sMessage ,*this);
                }

                 xFileAccess->writeFile(m_sFBKPath,xDBStream->getInputStream());
            }
            // TODO: Get DB properties from XML

        }
        // External file AND/OR remote connection
        else if (url.startsWith("sdbc:firebird:"))
        {
            m_sFirebirdURL = url.copy(OUString("sdbc:firebird:").getLength());
            if (m_sFirebirdURL.startsWith("file://"))
            {
                m_bIsFile = true;
                uno::Reference< ucb::XSimpleFileAccess > xFileAccess(
                    ucb::SimpleFileAccess::create(comphelper::getProcessComponentContext()),
                    uno::UNO_QUERY);
                if (!xFileAccess->exists(m_sFirebirdURL))
                    bIsNewDatabase = true;

                m_sFirebirdURL = m_sFirebirdURL.copy(OUString("file://").getLength());
            }
        }

        char dpbBuffer[1 + 3 + 257 + 257 ]; // Expand as needed
        int dpbLength = 0;
        {
            char* dpb;
            char userName[256] = "";
            char userPassword[256] = "";

            dpb = dpbBuffer;
            *dpb++ = isc_dpb_version1;

            *dpb++ = isc_dpb_sql_dialect;
            *dpb++ = 1; // 1 byte long
            *dpb++ = FIREBIRD_SQL_DIALECT;
            // Do any more dpbBuffer additions here

            if (m_bIsEmbedded || m_bIsFile)
            {
                *dpb++ = isc_dpb_trusted_auth;
                *dpb++ = 1; // Length of data
                *dpb++ = 1; // TRUE
            }
            else
            {
                // TODO: parse password from connection string as needed?
            }

            if (strlen(userName))
            {
                int nUsernameLength = strlen(userName);
                *dpb++ = isc_dpb_user_name;
                *dpb++ = (char) nUsernameLength;
                strcpy(dpb, userName);
                dpb+= nUsernameLength;
            }

            if (strlen(userPassword))
            {
                int nPasswordLength = strlen(userPassword);
                *dpb++ = isc_dpb_password;
                *dpb++ = (char) nPasswordLength;
                strcpy(dpb, userPassword);
                dpb+= nPasswordLength;
            }

            dpbLength = dpb - dpbBuffer;
        }

        ISC_STATUS_ARRAY status;            /* status vector */
        ISC_STATUS aErr;
        if (bIsNewDatabase)
        {
            aErr = isc_create_database(status,
                                       m_sFirebirdURL.getLength(),
                                       OUStringToOString(m_sFirebirdURL,RTL_TEXTENCODING_UTF8).getStr(),
                                       &m_aDBHandle,
                                       dpbLength,
                                       dpbBuffer,
                                       0);
            if (aErr)
            {
                evaluateStatusVector(status, "isc_create_database", *this);
            }
        }
        else
        {
            if (m_bIsEmbedded) // We need to restore the .fbk first
            {
                runBackupService(isc_action_svc_restore);
            }
            aErr = isc_attach_database(status,
                                       m_sFirebirdURL.getLength(),
                                       OUStringToOString(m_sFirebirdURL, RTL_TEXTENCODING_UTF8).getStr(),
                                       &m_aDBHandle,
                                       dpbLength,
                                       dpbBuffer);
            if (aErr)
            {
                evaluateStatusVector(status, "isc_attach_database", *this);
            }
        }

        if (m_bIsEmbedded) // Add DocumentEventListener to save the .fdb as needed
        {


            // We need to attach as a document listener in order to be able to store
            // the temporary db back into the .odb when saving
            uno::Reference<XDocumentEventBroadcaster> xBroadcaster(m_xParentDocument, UNO_QUERY);

            if (xBroadcaster.is())
                xBroadcaster->addDocumentEventListener(this);
            else
                assert(false);
        }
    }
    catch (const Exception&)
    {
        throw;
    }
    catch (const std::exception&)
    {
        throw;
    }
    catch (...) // const Firebird::Exception& firebird throws this, but doesn't install the fb_exception.h that declares it

    {
        throw std::runtime_error("Generic Firebird::Exception");
    }
}

void Connection::notifyDatabaseModified()
{
    if (m_xParentDocument.is()) // Only true in embedded mode
        m_xParentDocument->setModified(sal_True);
}

//----- XServiceInfo ---------------------------------------------------------
IMPLEMENT_SERVICE_INFO(Connection, "com.sun.star.sdbc.drivers.firebird.Connection",
                                                    "com.sun.star.sdbc.Connection")

Reference< XBlob> Connection::createBlob(ISC_QUAD* pBlobId)
    throw(SQLException, RuntimeException)
{
    MutexGuard aGuard(m_aMutex);
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    Reference< XBlob > xReturn = new Blob(&m_aDBHandle,
                                          &m_aTransactionHandle,
                                          *pBlobId);

    m_aStatements.push_back(WeakReferenceHelper(xReturn));
    return xReturn;
}


//----- XConnection ----------------------------------------------------------
Reference< XStatement > SAL_CALL Connection::createStatement( )
                                        throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    // the pre
    if(m_aTypeInfo.empty())
        buildTypeInfo();

    // create a statement
    // the statement can only be executed once
    Reference< XStatement > xReturn = new OStatement(this);
    m_aStatements.push_back(WeakReferenceHelper(xReturn));
    return xReturn;
}

OUString Connection::transformPreparedStatement(const OUString& _sSQL)
{
    OUString sSqlStatement (_sSQL);
    try
    {
        OSQLParser aParser( m_xDriver->getContext() );
        OUString sErrorMessage;
        OUString sNewSql;
        OSQLParseNode* pNode = aParser.parseTree(sErrorMessage,_sSQL);
        if(pNode)
        {   // special handling for parameters
            OSQLParseNode::substituteParameterNames(pNode);
            pNode->parseNodeToStr( sNewSql, this );
            delete pNode;
            sSqlStatement = sNewSql;
        }
    }
    catch(const Exception&)
    {
        SAL_WARN("connectivity.firebird", "failed to remove named parameters from '" << _sSQL << "'");
    }
    return sSqlStatement;
}

Reference< XPreparedStatement > SAL_CALL Connection::prepareStatement(
            const OUString& _sSql)
    throw(SQLException, RuntimeException, std::exception)
{
    SAL_INFO("connectivity.firebird", "prepareStatement() "
             "called with sql: " << _sSql);
    MutexGuard aGuard(m_aMutex);
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    if(m_aTypeInfo.empty())
        buildTypeInfo();

    OUString sSqlStatement (transformPreparedStatement( _sSql ));

    Reference< XPreparedStatement > xReturn = new OPreparedStatement(this,
                                                                     m_aTypeInfo,
                                                                     sSqlStatement);
    m_aStatements.push_back(WeakReferenceHelper(xReturn));

    return xReturn;
}

Reference< XPreparedStatement > SAL_CALL Connection::prepareCall(
                const OUString& _sSql ) throw(SQLException, RuntimeException, std::exception)
{
    SAL_INFO("connectivity.firebird", "prepareCall(). "
             "_sSql: " << _sSql);

    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    // OUString sSqlStatement (transformPreparedStatement( _sSql ));

    // not implemented yet :-) a task to do
    return NULL;
}

OUString SAL_CALL Connection::nativeSQL( const OUString& _sSql )
                                        throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    // We do not need to adapt the SQL for Firebird atm.
    return _sSql;
}

void SAL_CALL Connection::setAutoCommit( sal_Bool autoCommit )
                                        throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    m_bIsAutoCommit = autoCommit;

    if (m_aTransactionHandle)
    {
        setupTransaction();
    }
}

sal_Bool SAL_CALL Connection::getAutoCommit() throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    return m_bIsAutoCommit;
}

void Connection::setupTransaction()
    throw (SQLException)
{
    MutexGuard aGuard( m_aMutex );
    ISC_STATUS status_vector[20];

    // TODO: is this sensible? If we have changed parameters then transaction
    // is lost...
    if (m_aTransactionHandle)
    {
        disposeStatements();
        isc_rollback_transaction(status_vector, &m_aTransactionHandle);
    }

    char aTransactionIsolation = 0;
    switch (m_aTransactionIsolation)
    {
        // TODO: confirm that these are correct.
        case(TransactionIsolation::READ_UNCOMMITTED):
            aTransactionIsolation = isc_tpb_concurrency;
            break;
        case(TransactionIsolation::READ_COMMITTED):
            aTransactionIsolation = isc_tpb_read_committed;
            break;
        case(TransactionIsolation::REPEATABLE_READ):
            aTransactionIsolation = isc_tpb_consistency;
            break;
        case(TransactionIsolation::SERIALIZABLE):
            aTransactionIsolation = isc_tpb_consistency;
            break;
        default:
            assert( false ); // We must have a valid TransactionIsolation.
    }

    // You cannot pass an empty tpb parameter so we have to do some pointer
    // arithmetic to avoid problems. (i.e. aTPB[x] = 0 is invalid)
    char aTPB[5];
    char* pTPB = aTPB;

    *pTPB++ = isc_tpb_version3;
    if (m_bIsAutoCommit)
        *pTPB++ = isc_tpb_autocommit;
    *pTPB++ = (!m_bIsReadOnly ? isc_tpb_write : isc_tpb_read);
    *pTPB++ = aTransactionIsolation;
    *pTPB++ = isc_tpb_wait;

    isc_start_transaction(status_vector,
                          &m_aTransactionHandle,
                          1,
                          &m_aDBHandle,
                          pTPB - aTPB, // bytes used in TPB
                          aTPB);

    evaluateStatusVector(status_vector,
                         "isc_start_transaction",
                         *this);
}

isc_tr_handle& Connection::getTransaction()
    throw (SQLException)
{
    MutexGuard aGuard( m_aMutex );
    if (!m_aTransactionHandle)
    {
        setupTransaction();
    }
    return m_aTransactionHandle;
}

isc_svc_handle Connection::attachServiceManager()
{
    ISC_STATUS_ARRAY aStatusVector;
    isc_svc_handle aServiceHandle = 0;

    char aSPBBuffer[] = { isc_spb_version, isc_spb_current_version};

    if (isc_service_attach(aStatusVector,
                            0, // Denotes null-terminated string next
                            "service_mgr",
                            &aServiceHandle,
                            sizeof(aSPBBuffer),
                            aSPBBuffer))
    {
        evaluateStatusVector(aStatusVector,
                             "isc_service_attach",
                             *this);
    }

    return aServiceHandle;
}

void Connection::detachServiceManager(isc_svc_handle aServiceHandle)
{
    ISC_STATUS_ARRAY aStatusVector;
    if (isc_service_detach(aStatusVector,
                            &aServiceHandle))
    {
        evaluateStatusVector(aStatusVector,
                             "isc_service_detach",
                             *this);
    }
}

void Connection::runBackupService(const short nAction)
{
    assert(nAction == isc_action_svc_backup
           || nAction == isc_action_svc_restore);

    ISC_STATUS_ARRAY aStatusVector;

    char sRequest[200];
    char* pRequest = sRequest;

    *pRequest++ = nAction;

    *pRequest++ = isc_spb_dbname;  // The .fdb
    sal_uInt16 nURLLength = m_sFirebirdURL.getLength();
    *((sal_uInt16*) pRequest) = nURLLength;
    pRequest += 2;
    strncpy(pRequest,
            OUStringToOString(m_sFirebirdURL,
                              RTL_TEXTENCODING_UTF8).getStr(),
            nURLLength);
    pRequest += nURLLength;

    *pRequest++ = isc_spb_bkp_file; // The fbk
    sal_uInt16 nPathLength = m_sFBKPath.getLength();
    *((sal_uInt16*) pRequest) = nPathLength;
    pRequest += 2;
    strncpy(pRequest,
            OUStringToOString(m_sFBKPath,
                              RTL_TEXTENCODING_UTF8).getStr(),
            nPathLength);
    pRequest += nPathLength;

    // TODO: not sure we really need this yet -- assumed by default?
    if (nAction == isc_action_svc_restore)
    {
        *pRequest++ = isc_spb_options;
        // This is a 4 byte value (the docs suggest unsigned long...)
        *((sal_uInt32*) pRequest) = isc_spb_res_create;
        pRequest += 4;
    }

    *pRequest++ = isc_spb_verbose;

    isc_svc_handle aServiceHandle = attachServiceManager();

    if (isc_service_start(aStatusVector,
                          &aServiceHandle,
                          NULL,
                          pRequest - sRequest,
                          sRequest))
    {
        evaluateStatusVector(aStatusVector, "isc_service_start", *this);
    }

    while (true)
    {
        char aInfoSPB = isc_info_svc_line;
        char aResults[512];
        char* pResults = aResults;
        isc_service_query(aStatusVector,
                          &aServiceHandle,
                          0, // Reserved null
                          0,0, // "send" spb -- size and spb -- not needed?
                          1,
                          &aInfoSPB,
                          sizeof(aResults),
                          aResults);
        if (isc_vax_integer(pResults, 1) == isc_info_svc_line)
        {
            if (isc_vax_integer(pResults + 1, 2) == 0) // Empty string == command finished
                break;

            OUString aData(pResults + 3,
                           isc_vax_integer(pResults, 2),
                           RTL_TEXTENCODING_UTF8);
            SAL_INFO("connectivity.firebird", "backupService: " << aData);
        }
        else if (isc_vax_integer(pResults, 2) == isc_info_truncated)
        {
            SAL_INFO("connectivity.firebird", "backupService output truncated");
            break;
        }

    }

    detachServiceManager(aServiceHandle);
}

void SAL_CALL Connection::commit() throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    ISC_STATUS status_vector[20];

    if (!m_bIsAutoCommit && m_aTransactionHandle)
    {
        disposeStatements();
        isc_commit_transaction(status_vector, &m_aTransactionHandle);
        evaluateStatusVector(status_vector,
                             "isc_commit_transaction",
                             *this);
    }
}

void SAL_CALL Connection::rollback() throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    ISC_STATUS status_vector[20];

    if (!m_bIsAutoCommit && m_aTransactionHandle)
    {
        isc_rollback_transaction(status_vector, &m_aTransactionHandle);
    }
}

sal_Bool SAL_CALL Connection::isClosed(  ) throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );

    // just simple -> we are close when we are disposed that means someone called dispose(); (XComponent)
    return Connection_BASE::rBHelper.bDisposed;
}

Reference< XDatabaseMetaData > SAL_CALL Connection::getMetaData(  ) throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    // here we have to create the class with biggest interface
    // The answer is 42 :-)
    Reference< XDatabaseMetaData > xMetaData = m_xMetaData;
    if(!xMetaData.is())
    {
        xMetaData = new ODatabaseMetaData(this); // need the connection because it can return it
        m_xMetaData = xMetaData;
    }

    return xMetaData;
}

void SAL_CALL Connection::setReadOnly(sal_Bool readOnly)
                                            throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    m_bIsReadOnly = readOnly;
    setupTransaction();
}

sal_Bool SAL_CALL Connection::isReadOnly() throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    return m_bIsReadOnly;
}

void SAL_CALL Connection::setCatalog(const OUString& /*catalog*/)
    throw(SQLException, RuntimeException, std::exception)
{
    ::dbtools::throwFunctionNotSupportedSQLException("setCatalog", *this);
}

OUString SAL_CALL Connection::getCatalog()
    throw(SQLException, RuntimeException, std::exception)
{
    ::dbtools::throwFunctionNotSupportedSQLException("getCatalog", *this);
    return OUString();
}

void SAL_CALL Connection::setTransactionIsolation( sal_Int32 level ) throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    m_aTransactionIsolation = level;
    setupTransaction();
}

sal_Int32 SAL_CALL Connection::getTransactionIsolation(  ) throw(SQLException, RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );
    checkDisposed(Connection_BASE::rBHelper.bDisposed);

    return m_aTransactionIsolation;
}

Reference< XNameAccess > SAL_CALL Connection::getTypeMap() throw(SQLException, RuntimeException, std::exception)
{
    ::dbtools::throwFeatureNotImplementedSQLException( "XConnection::getTypeMap", *this );
    return 0;
}

void SAL_CALL Connection::setTypeMap(const Reference< XNameAccess >& typeMap)
                                            throw(SQLException, RuntimeException, std::exception)
{
    ::dbtools::throwFeatureNotImplementedSQLException( "XConnection::setTypeMap", *this );
    (void) typeMap;
}

//----- XCloseable -----------------------------------------------------------
void SAL_CALL Connection::close(  ) throw(SQLException, RuntimeException, std::exception)
{
    // we just dispose us
    {
        MutexGuard aGuard( m_aMutex );
        checkDisposed(Connection_BASE::rBHelper.bDisposed);

    }
    dispose();
}

// XWarningsSupplier
Any SAL_CALL Connection::getWarnings(  ) throw(SQLException, RuntimeException, std::exception)
{
    // when you collected some warnings -> return it
    return Any();
}

void SAL_CALL Connection::clearWarnings(  ) throw(SQLException, RuntimeException, std::exception)
{
    // you should clear your collected warnings here
}

// XDocumentEventListener
void SAL_CALL Connection::documentEventOccured( const DocumentEvent& _Event )
                                                        throw(RuntimeException, std::exception)
{
    MutexGuard aGuard(m_aMutex);

    if (!m_bIsEmbedded)
        return;

    if (_Event.EventName == "OnSave" || _Event.EventName == "OnSaveAs")
    {
        commit(); // Commit and close transaction
        if ( m_bIsEmbedded && m_xEmbeddedStorage.is() )
        {
            SAL_INFO("connectivity.firebird", "Writing .fbk from running db");
            runBackupService(isc_action_svc_backup);

            SAL_INFO("connectivity.firebird", "Writing .fdk into .odb" );
            Reference< XStream > xDBStream(m_xEmbeddedStorage->openStreamElement(our_sFBKLocation,
                                                                       ElementModes::WRITE));
            // TODO: verify the backup actually exists -- the backup service
            // can fail without giving any sane error messages / telling us
            // that it failed.
            using namespace ::comphelper;
            Reference< XComponentContext > xContext = comphelper::getProcessComponentContext();
            Reference< XInputStream > xInputStream;
            if (xContext.is())
                xInputStream =
                        OStorageHelper::GetInputStreamFromURL(m_sFBKPath, xContext);
            if (xInputStream.is())
                OStorageHelper::CopyInputToOutput( xInputStream,
                                                xDBStream->getOutputStream());
        }
    }
}
// XEventListener
void SAL_CALL Connection::disposing(const EventObject& /*rSource*/)
    throw (RuntimeException, std::exception)
{
    MutexGuard aGuard( m_aMutex );

    m_xEmbeddedStorage.clear();
}

void Connection::buildTypeInfo() throw( SQLException)
{
    MutexGuard aGuard( m_aMutex );

    Reference< XResultSet> xRs = getMetaData ()->getTypeInfo ();
    Reference< XRow> xRow(xRs,UNO_QUERY);
    // Information for a single SQL type

    // Loop on the result set until we reach end of file

    while (xRs->next ())
    {
        OTypeInfo aInfo;
        aInfo.aTypeName         = xRow->getString   (1);
        aInfo.nType             = xRow->getShort    (2);
        aInfo.nPrecision        = xRow->getInt      (3);
        aInfo.aLiteralPrefix    = xRow->getString   (4);
        aInfo.aLiteralSuffix    = xRow->getString   (5);
        aInfo.aCreateParams     = xRow->getString   (6);
        aInfo.bNullable         = xRow->getBoolean  (7);
        aInfo.bCaseSensitive    = xRow->getBoolean  (8);
        aInfo.nSearchType       = xRow->getShort    (9);
        aInfo.bUnsigned         = xRow->getBoolean  (10);
        aInfo.bCurrency         = xRow->getBoolean  (11);
        aInfo.bAutoIncrement    = xRow->getBoolean  (12);
        aInfo.aLocalTypeName    = xRow->getString   (13);
        aInfo.nMinimumScale     = xRow->getShort    (14);
        aInfo.nMaximumScale     = xRow->getShort    (15);
        aInfo.nNumPrecRadix     = (sal_Int16)xRow->getInt(18);



        // Now that we have the type info, save it
        // in the Hashtable if we don't already have an
        // entry for this SQL type.

        m_aTypeInfo.push_back(aInfo);
    }

    SAL_INFO("connectivity.firebird", "buildTypeInfo(). "
             "Type info built.");

    // Close the result set/statement.

    Reference< XCloseable> xClose(xRs,UNO_QUERY);
    xClose->close();

    SAL_INFO("connectivity.firebird", "buildTypeInfo(). "
             "Closed.");
}

void Connection::disposing()
{
    MutexGuard aGuard(m_aMutex);

    disposeStatements();

    m_xMetaData = ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XDatabaseMetaData>();

    ISC_STATUS_ARRAY status;            /* status vector */
    if (m_aTransactionHandle)
    {
        // TODO: confirm whether we need to ask the user here.
        isc_rollback_transaction(status, &m_aTransactionHandle);
    }

    if (isc_detach_database(status, &m_aDBHandle))
    {
        evaluateStatusVector(status, "isc_detach_database", *this);
    }
    // TODO: write to storage again?

    dispose_ChildImpl();
    cppu::WeakComponentImplHelperBase::disposing();
    m_xDriver.clear();

    if (m_pDatabaseFileDir)
    {
        ::utl::removeTree(m_pDatabaseFileDir->GetURL());
        m_pDatabaseFileDir.reset();
    }
}

void Connection::disposeStatements()
{
    MutexGuard aGuard(m_aMutex);
    for (OWeakRefArray::iterator i = m_aStatements.begin(); m_aStatements.end() != i; ++i)
    {
        Reference< XComponent > xComp(i->get(), UNO_QUERY);
        if (xComp.is())
            xComp->dispose();
    }
    m_aStatements.clear();
}

uno::Reference< XTablesSupplier > Connection::createCatalog()
{
    MutexGuard aGuard(m_aMutex);

    // m_xCatalog is a weak reference. Reuse it if it still exists.
    Reference< XTablesSupplier > xCatalog = m_xCatalog;
    if (xCatalog.is())
    {
        return xCatalog;
    }
    else
    {
        xCatalog = new Catalog(this);
        m_xCatalog = xCatalog;
        return m_xCatalog;
    }

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
