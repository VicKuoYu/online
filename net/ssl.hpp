/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SSL_HPP
#define INCLUDED_SSL_HPP

#include <atomic>
#include <memory>
#include <string>

#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#if OPENSSL_VERSION_NUMBER >= 0x0907000L
#include <openssl/conf.h>
#endif

class SslContext
{
public:
    static void initialize(const std::string& certFilePath,
                           const std::string& keyFilePath,
                           const std::string& caFilePath)
    {
        if (++RefCount == 1)
        {
            Instance.reset(new SslContext(certFilePath, keyFilePath, caFilePath));
        }
    }

    static void uninitialize()
    {
        if (--RefCount == 0)
        {
            Instance.reset();
        }
    }

    static SSL* newSsl()
    {
        return SSL_new(Instance->_ctx);
    }

    ~SslContext();

private:
    SslContext(const std::string& certFilePath,
               const std::string& keyFilePath,
               const std::string& caFilePath);

private:
    static std::atomic<int> RefCount;
    static std::unique_ptr<SslContext> Instance;

    SSL_CTX* _ctx;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */