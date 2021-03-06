/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Runs client tests in their own thread inside a WSD process.

#include "config.h"

#include "Unit.hpp"
#include "wsd/LOOLWSD.hpp"

#include "test.hpp"

// Inside the WSD process
class UnitClient : public UnitWSD
{
    bool _workerStarted;
    std::thread _worker;

public:
    UnitClient() :
        _workerStarted(false)
    {
        int timeout_minutes = 5;
        setTimeout(timeout_minutes * 60 * 1000);
    }
    ~UnitClient()
    {
        LOG_INF("Joining test worker thread\n");
        _worker.join();
    }

    void configure(Poco::Util::LayeredConfiguration& config) override
    {
        UnitWSD::configure(config);
        // force HTTPS - to test harder
        config.setBool("ssl.enable", true);
    }

    void invokeTest() override
    {
        // this method gets called every few seconds.
        if (_workerStarted)
            return;
        _workerStarted = true;

        _worker = std::thread([this]{
                if (runClientTests(false, true))
                    exitTest (TestResult::Failed);
                else
                    exitTest (TestResult::Ok);
            });
    }
};

UnitBase *unit_create_wsd(void)
{
    return new UnitClient();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
