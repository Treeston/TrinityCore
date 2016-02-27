-- custom dummy 1
-- dps check: 2750 dps sustained
-- weakening bar: 60 seconds normal, 15 seconds weakened (first weaken after 20 sec)
-- enrage after 4 minutes 20 seconds (end of 4th weakened phase)
SET @BASEENTRY = 100000;
SET @BASECGUID = 1500000;
SET @BASEOGUID = 1500000;

DELETE FROM `creature_template` WHERE `entry` between @BASEENTRY and @BASEENTRY;
INSERT INTO `creature_template` (`entry`,`modelid1`,`name`,`subname`,`minlevel`,`maxlevel`,`faction`,`speed_walk`,`speed_run`,`scale`,`rank`,`BaseAttackTime`,`BaseVariance`,`unit_class`,`unit_flags`,`unit_flags2`,`type`,`type_flags`,`mingold`,`maxgold`,`ScriptName`,`MovementType`,`InhabitType`,`HealthModifier`,`ExperienceModifier`,`mechanic_immune_mask`,`flags_extra`) VALUES
                             (@BASEENTRY,16174,"The Wrecker", "Abominable Abomination",83,83,     14,           2,       0.15, 1.5   ,     3,             850,             1,           1,           0,            0,     6,          76,        1,        1,"boss_custom_dps1",       0,            1,         123.139,                   0,            2147483647,       262484);
					
DELETE FROM `creature` WHERE `guid` between @BASECGUID and @BASECGUID;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`movementtype`) VALUES
                   (@BASECGUID,@BASEENTRY,169,     15,          1,        0,      -145.9,      2454.2,   92.007912,3.14159265359,             30,             0);
				   
DELETE FROM `creature_text` WHERE `entry` = @BASEENTRY;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`probability`,`TextRange`) VALUES
(@BASEENTRY, 1,0,"Little $n watch while I smash!",14,100,3),
(@BASEENTRY, 2,0,"%s begins to charge the wall! Take it down before it can get there!",41,100,3),
(@BASEENTRY, 3,0,"Tired. So...tired.",12,100,3),
(@BASEENTRY, 4,0,"%s is weakened while taking a rest. Strike now!",41,100,3),
(@BASEENTRY, 5,0,"Coming for you now, puny mortals!",14,100,3),
(@BASEENTRY, 6,0,"%s charges at the barrier with renewed strength!",41,100,3),
(@BASEENTRY, 7,0,"Wrecker tired of games. Now, smash!",14,100,3),
(@BASEENTRY, 8,0,"%s begins to relentlessly pound the wall! Take it down!",41,100,3),
(@BASEENTRY, 9,0,"Hmph. Friend will not help you here.",14,100,3),
(@BASEENTRY,10,0,"Why... So tired....",12,100,3);
