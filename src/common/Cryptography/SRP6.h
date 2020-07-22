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

#ifndef TRINITY_SRP6_H
#define TRINITY_SRP6_H

#include "BigNumber.h"
#include "Define.h"
#include "SHA1.h"
#include <ctime>
#include <optional>
#include <vector>

namespace Trinity
{
namespace Crypto
{
class TC_COMMON_API SRP6
{
    public:
        static constexpr uint32 SEED_LENGTH_BYTES = 32;
        static constexpr int32 SEED_LENGTH_BITS = SEED_LENGTH_BYTES * 8;

        static constexpr uint32 VERIFIER_LENGTH_BYTES = 32;
        static constexpr int32 VERIFIER_LENGTH_BITS = VERIFIER_LENGTH_BYTES * 8;

        static const BigNumber g, k, N;

        static BigNumber CalculateVerifier(BigNumber seed, std::string const& username, std::string const& password);

        enum class Progress
        {
            PENDING_INIT,
            PENDING_PROOF,
        };

        struct InitializeResult
        {
            BigNumber const seed;
            BigNumber const B;
        };
        InitializeResult Initialize(BigNumber const& seed, BigNumber const& verifier);

        using VerifyResult = std::optional<SHA1Hash::Digest>;
        VerifyResult VerifyProof(std::string const& username, BigNumber const& A, BigNumber const& M1);

    private:
        Progress _progress = Progress::PENDING_INIT;
        BigNumber _seed, _verifier, _b, _B, _u;
};
}
}

#endif
