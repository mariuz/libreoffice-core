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

#ifndef INCLUDED_BASEGFX_VECTOR_B2ENUMS_HXX
#define INCLUDED_BASEGFX_VECTOR_B2ENUMS_HXX

#include <sal/types.h>

namespace basegfx
{
    /** Descriptor for the mathematical orientations of two 2D Vectors
    */
    enum class B2VectorOrientation
    {
        /// mathematically positive oriented
        Positive = 0,

        /// mathematically negative oriented
        Negative,

        /// mathematically neutral, thus parallel
        Neutral
    };

    /** Descriptor for the mathematical continuity of two 2D Vectors
    */
    enum B2VectorContinuity
    {
        /// none
        NONE = 0,

        /// mathematically negative oriented
        C1,

        /// mathematically neutral, thus parallel
        C2
    };

    /** Descriptor for possible line joins between two line segments
    */
    enum class B2DLineJoin
    {
        NONE,       // no rounding
        Middle,     // calc middle value between joints
        Bevel,      // join edges with line
        Miter,      // extend till cut
        Round       // create arc
    };

} // end of namespace basegfx

#endif // INCLUDED_BASEGFX_VECTOR_B2ENUMS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
