/*
* Copyright (C) 2016-2016 TrinityCore <http://www.trinitycore.org/>
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

/*
* Ordered alphabetically using scriptname.
* Scriptnames of files in this file should be prefixed with "npc_pet_warlock_".
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PetAI.h"

class npc_pet_warlock_imp : public CreatureScript
{
    public:
        npc_pet_warlock_imp() : CreatureScript("npc_pet_warlock_imp") { }

        struct npc_pet_warlock_impAI : public PetAI
        {
            npc_pet_warlock_impAI(Creature* creature) : PetAI(creature) { }

            bool ShouldMeleeAttack(Unit* /*target*/) override {  return false; }
            float DetermineChaseDist(Unit* /*target*/) override { return 25.0f; }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_pet_warlock_impAI(creature);
        }
};

void AddSC_warlock_pet_scripts()
{
    new npc_pet_warlock_imp();
}
