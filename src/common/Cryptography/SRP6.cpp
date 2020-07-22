/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SRP6.h"
#include "Errors.h"
#include "SHA1.h"

/*static*/ const BigNumber Trinity::Crypto::SRP6::g = 7;
/*static*/ const BigNumber Trinity::Crypto::SRP6::k = 3;
/*static*/ const BigNumber Trinity::Crypto::SRP6::N = "894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7";

/*static*/ BigNumber Trinity::Crypto::SRP6::CalculateVerifier(BigNumber seed, std::string const& username, std::string const& password)
{
    SHA1Hash shaA, shaB;
    shaA.UpdateData(username);
    shaA.UpdateData(":");
    shaA.UpdateData(password);
    shaA.Finalize();

    shaB.UpdateData(seed.AsByteArray(SEED_LENGTH_BYTES).get(), SEED_LENGTH_BYTES);
    shaB.UpdateData(shaA.GetDigest());
    shaB.Finalize();

    BigNumber x;
    x.SetBinary(shaB.GetDigest());
    return g.ModExp(x, N);
}

Trinity::Crypto::SRP6::InitializeResult Trinity::Crypto::SRP6::Initialize(BigNumber const& seed, BigNumber const& verifier)
{
    ASSERT(_progress == Progress::PENDING_INIT);

    _seed = seed;
    _verifier = verifier;
    _b.SetRand(19 * 8);
    _B = (((_verifier * 3) + g.ModExp(_b, N)) % N);

    _progress = Progress::PENDING_PROOF;

    return { _seed, _B };
}

Trinity::Crypto::SRP6::VerifyResult Trinity::Crypto::SRP6::VerifyProof(std::string const& username, BigNumber const& A, BigNumber const& M1)
{
    ASSERT(_progress == Progress::PENDING_PROOF);
    if ((A % N).IsZero())
        return std::nullopt;
    SHA1Hash sha;
    sha.UpdateBigNumbers(A, _B);
    sha.Finalize();
    BigNumber u;
    u.SetBinary(sha.GetDigest());
    BigNumber S = (A * (_verifier.ModExp(u, N))).ModExp(_b, N);

    std::unique_ptr<uint8[]> t = S.AsByteArray(32);
    std::array<uint8,16> t1;
    std::array<uint8,40> vK;

    for (size_t i = 0; i < 16; ++i)
        t1[i] = t[i * 2];

    sha.Initialize();
    sha.UpdateData(t1);
    sha.Finalize();

    for (size_t i = 0; i < 20; ++i)
        vK[i * 2] = sha.GetDigest()[i];

    for (size_t i = 0; i < 16; ++i)
        t1[i] = t[i * 2 + 1];

    sha.Initialize();
    sha.UpdateData(t1);
    sha.Finalize();

    for (size_t i = 0; i < 20; ++i)
        vK[i * 2 + 1] = sha.GetDigest()[i];

    BigNumber K;
    K.SetBinary(vK);

    sha.Initialize();
    sha.UpdateBigNumbers(N);
    sha.Finalize();
    auto hash = sha.GetDigest();

    sha.Initialize();
    sha.UpdateBigNumbers(g);
    sha.Finalize();

    for (size_t i = 0; i < 20; ++i)
        hash[i] ^= sha.GetDigest()[i];

    BigNumber t3;
    t3.SetBinary(hash);

    sha.Initialize();
    sha.UpdateData(username);
    sha.Finalize();
    auto t4 = sha.GetDigest();

    sha.Initialize();
    sha.UpdateBigNumbers(t3);
    sha.UpdateData(t4);
    sha.UpdateBigNumbers(_seed, A, _B, K);
    sha.Finalize();
    BigNumber M;
    M.SetBinary(sha.GetDigest());

    if (M == M1)
    {
        sha.Initialize();
        sha.UpdateBigNumbers(A, M, K);
        sha.Finalize();
        return sha.GetDigest();
    }
    else
        return std::nullopt;
}
