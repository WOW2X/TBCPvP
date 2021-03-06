 /*
  * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
  * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
  * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
  * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

/* ScriptData
SDName: Boss_Arlokk
SD%Complete: 95
SDComment: Mark still not causing Panthers to attack a single target.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptPCH.h"
#include "zulgurub.h"

uint64 MarkedTargetGUID;
uint32 m_uiSummonCount;

enum eEnums
{
    SAY_AGGRO                   = -1309011,
    SAY_FEAST_PANTHER           = -1309012,
    SAY_DEATH                   = -1309013,

    SPELL_SHADOWWORDPAIN        = 24212,
    SPELL_GOUGE                 = 12540,
    SPELL_MARK                  = 24210,
    SPELL_WHIRLWIND             = 15589,
    SPELL_PANTHER_TRANSFORM     = 24190,
    SPELL_RAVAGE                = 24213,
    SPELL_THRASH                = 3391,
    SPELL_BACKSTAB              = 15582,

    MODEL_ID_NORMAL             = 15218,
    MODEL_ID_PANTHER            = 15215,
    MODEL_ID_BLANK              = 11686,

    NPC_ZULIAN_PROWLER          = 15101
};

struct boss_arlokkAI : public ScriptedAI
{
    boss_arlokkAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    ScriptedInstance *instance;

    uint32 m_uiShadowWordPain_Timer;
    uint32 m_uiGouge_Timer;
    uint32 m_uiMark_Timer;
    uint32 m_uiWhirlwind_Timer;
    uint32 m_uiVanish_Timer;
    uint32 m_uiVisible_Timer;
    uint32 m_uiThrash_Timer;
    uint32 m_uiRavage_Timer;

    uint32 m_uiSummon_Timer;

    bool m_bIsPhaseTwo;
    bool m_bIsVanished;

    void Reset()
    {
        m_uiShadowWordPain_Timer = 8000;
        m_uiGouge_Timer = 14000;
        m_uiMark_Timer = 35000;
        m_uiWhirlwind_Timer = 4000;
        m_uiVanish_Timer = 60000;
        m_uiVisible_Timer = 6000;
        m_uiThrash_Timer = urand(5000, 9000);
        m_uiRavage_Timer = 15000;

        m_uiSummon_Timer = 5000;
        m_uiSummonCount = 0;

        m_bIsPhaseTwo = false;
        m_bIsVanished = false;

        MarkedTargetGUID = 0;

        me->SetDisplayId(MODEL_ID_NORMAL);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustReachedHome()
    {
        if (instance)
            instance->SetData(TYPE_ARLOKK, NOT_STARTED);

        // boss is summoned, so despawn
        me->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, me);

        me->SetDisplayId(MODEL_ID_NORMAL);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);

        if (instance)
            instance->SetData(TYPE_ARLOKK, DONE);
    }

    void DoSummonPhanters()
    {
        if (Unit *pMarkedTarget = Unit::GetUnit(*me, MarkedTargetGUID))
            DoScriptText(SAY_FEAST_PANTHER, me, pMarkedTarget);

        me->SummonCreature(NPC_ZULIAN_PROWLER, -11532.7998, -1649.6734, 41.4800, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
        me->SummonCreature(NPC_ZULIAN_PROWLER, -11532.9970, -1606.4840, 41.2979, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit *pMarkedTarget = Unit::GetUnit(*me, MarkedTargetGUID))
            pSummoned->AI()->AttackStart(pMarkedTarget);
        else
        {
            if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                pSummoned->AI()->AttackStart(pTarget);
        }

        ++m_uiSummonCount;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (!m_bIsPhaseTwo)
        {
            if (m_uiShadowWordPain_Timer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_SHADOWWORDPAIN);
                m_uiShadowWordPain_Timer = 15000;
            }
            else
                m_uiShadowWordPain_Timer -= uiDiff;

            if (m_uiMark_Timer <= uiDiff)
            {
                Unit *pMarkedTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

                if (pMarkedTarget)
                {
                    DoCast(pMarkedTarget, SPELL_MARK);
                    MarkedTargetGUID = pMarkedTarget->GetGUID();
                }
                else
                    sLog->outError("TSCR: boss_arlokk could not acquire pMarkedTarget.");

                m_uiMark_Timer = 15000;
            }
            else
                m_uiMark_Timer -= uiDiff;
        }
        else
        {
            // Thrash Timer
            if (m_uiThrash_Timer <= uiDiff)
            {
                DoCast(me, SPELL_THRASH);
                m_uiThrash_Timer = urand(5000, 9000);
            }
            else
                m_uiThrash_Timer -= uiDiff;

           // Ravage Timer
           if (m_uiRavage_Timer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_RAVAGE);
                m_uiRavage_Timer = 16000;
            }
            else
                m_uiRavage_Timer -= uiDiff;

            // Whirlwind_Timer
            if (m_uiWhirlwind_Timer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_WHIRLWIND);
                m_uiWhirlwind_Timer = 16000;
            }
            else
                m_uiWhirlwind_Timer -= uiDiff;

            // Gouge_Timer
            if (m_uiGouge_Timer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_GOUGE);

                if (DoGetThreat(me->getVictim()))
                    DoModifyThreatPercent(me->getVictim(), -80);

                m_uiGouge_Timer = 17000 + rand()%10000;
            }
            else
                m_uiGouge_Timer -= uiDiff;
        }

        if (m_uiSummonCount <= 30)
        {
            if (m_uiSummon_Timer <= uiDiff)
            {
                DoSummonPhanters();
                m_uiSummon_Timer = 5000;
            }
            else
                m_uiSummon_Timer -= uiDiff;
        }

        if (m_uiVanish_Timer <= uiDiff)
        {
            // Invisible Model
            me->SetDisplayId(MODEL_ID_BLANK);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            me->AttackStop();
            DoResetThreat();

            m_bIsVanished = true;

            m_uiVanish_Timer = 45000;
            m_uiVisible_Timer = 6000;
        }
        else
            m_uiVanish_Timer -= uiDiff;

        if (m_bIsVanished)
        {
            if (m_uiVisible_Timer <= uiDiff)
            {
                // The Panther Model
                me->SetDisplayId(MODEL_ID_PANTHER);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                const CreatureTemplate *cinfo = me->GetCreatureTemplate();
                me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg / 100) * 35)));
                me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg / 100) * 35)));
                me->UpdateDamagePhysical(BASE_ATTACK);

                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                {
                    AttackStart(pTarget);
                    DoCast(pTarget, SPELL_BACKSTAB);
                }

                m_bIsPhaseTwo = true;
                m_bIsVanished = false;
                me->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.7f);
            }
            else
                m_uiVisible_Timer -= uiDiff;
        }
        else
            DoMeleeAttackIfReady();
    }
};

struct mob_prowlerAI : public ScriptedAI
{
    uint32 m_uiThrash_Timer;
    uint32 m_uiUpdateTarget_Timer;

    mob_prowlerAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    ScriptedInstance *instance;

    void Reset()
    {
        m_uiThrash_Timer = urand(5000, 9000);
        m_uiUpdateTarget_Timer = 2000;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        --m_uiSummonCount;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiUpdateTarget_Timer <= uiDiff)
        {
            Unit *pMarkedTarget = Unit::GetUnit(*me, MarkedTargetGUID);
            if (DoGetThreat(me->getVictim()))
                DoModifyThreatPercent(me->getVictim(), -100);
            me->AI()->AttackStart(pMarkedTarget);
            m_uiUpdateTarget_Timer = 2000;
        }
        else
            m_uiUpdateTarget_Timer -= uiDiff;

        if (m_uiThrash_Timer <= uiDiff)
        {
            DoCast(me, SPELL_THRASH);
            m_uiThrash_Timer = urand(5000, 9000);
        }
        else
            m_uiThrash_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_arlokk(Creature* creature)
{
    return new boss_arlokkAI (creature);
}

CreatureAI* GetAI_mob_prowler(Creature* creature)
{
    return new mob_prowlerAI (creature);
}

void AddSC_boss_arlokk()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_arlokk";
    newscript->GetAI = &GetAI_boss_arlokk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_prowler";
    newscript->GetAI = &GetAI_mob_prowler;
    newscript->RegisterSelf();
}

