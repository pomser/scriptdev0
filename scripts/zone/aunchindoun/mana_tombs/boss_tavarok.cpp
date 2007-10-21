/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "../../../sc_defines.h"

#define SPELL_EARTHQUAKE                33919
#define SPELL_CRYSTAL_PRISON            32361
#define SPELL_ARCING_SMASH              38761 //heroic only

struct MANGOS_DLL_DECL boss_tavarokAI : public ScriptedAI
{
    boss_tavarokAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}   

    uint32 earthquake_timer;
    uint32 crystalprison_timer;

    void EnterEvadeMode()
    {   
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);

        earthquake_timer = 10000;
        crystalprison_timer = 12000;

    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            DoStartMeleeAttack(who);
        }
    }

    void MoveInLineOfSight(Unit *who)
    {  
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && m_creature->IsWithinLOSInMap(who))
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH); 
                DoStartMeleeAttack(who);
            }
        }
    }  

    void UpdateAI(const uint32 diff)
    {

        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(earthquake_timer < diff)
        {     
            DoCast(m_creature,SPELL_EARTHQUAKE);
            earthquake_timer = 20000 + rand()%5000;
        }else earthquake_timer -= diff;

        if(crystalprison_timer < diff)
        {     
            Unit* target = NULL;
            target = SelectUnit(SELECT_TARGET_RANDOM,0);
            if(target && (target != m_creature->getVictim() ) ) 
            {
                DoCast(target,SPELL_CRYSTAL_PRISON);
                crystalprison_timer = 15000 + rand()%5000;
            }
        }else crystalprison_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_tavarokAI(Creature *_Creature)
{
    return new boss_tavarokAI (_Creature);
}

void AddSC_boss_tavarok()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_tavarok";
    newscript->GetAI = GetAI_boss_tavarokAI;
    m_scripts[nrscripts++] = newscript;
}