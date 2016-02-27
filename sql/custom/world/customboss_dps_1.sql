-- custom dummy 1
-- dps check: 2750 dps sustained
-- weakening bar: 60 seconds normal, 15 seconds weakened (first weaken after 20 sec)
-- enrage after 4 minutes 20 seconds (end of 4th weakened phase)
SET @BASECENTRY = 100000;
SET @BASEOENTRY = 1000000;
SET @BASECGUID = 1500000;
SET @BASEOGUID = 1500000;
SET @BASEGOSSIP = 60400;
SET @BASETEXT = 16740000;

DELETE FROM `creature_template` WHERE `entry` between @BASECENTRY+0 and @BASECENTRY+1;
INSERT INTO `creature_template` (`entry`,`modelid1`,`name`,`subname`,`minlevel`,`maxlevel`,`faction`,`npcflag`,`speed_walk`,`speed_run`,`scale`,`rank`,`BaseAttackTime`,`BaseVariance`,`unit_class`,`unit_flags`,`unit_flags2`,`type`,`type_flags`,`mingold`,`maxgold`,`ScriptName`,`MovementType`,`InhabitType`,`HealthModifier`,`ExperienceModifier`,`mechanic_immune_mask`,`flags_extra`) VALUES
(@BASECENTRY+0,16174,"The Wrecker","Abominable Abomination",83,83,14,0,2,0.15,1.5,3,850,1,1,0,0,6,76,1,1,"boss_custom_dps1",0,1,123.139,0,2147483647,262484),
(@BASECENTRY+1,27108,"Novarch","The Gate Watcher",83,83,35,1,2,2,0.1,3,0.5,1,1,770,0,5,135266324,0,0,"npc_custom_dps1_gatewatcher",0,1,50,0,2147483647,2);
							 
DELETE FROM `creature_text` WHERE `entry` = @BASECENTRY+0;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`probability`,`TextRange`) VALUES
(@BASECENTRY+0, 1,0,"Little $n watch while I smash!",14,100,3),
(@BASECENTRY+0, 2,0,"%s begins to charge the wall! Take it down before it can get there!",41,100,3),
(@BASECENTRY+0, 3,0,"Tired. So...tired.",12,100,3),
(@BASECENTRY+0, 4,0,"%s is weakened while taking a rest. Strike now!",41,100,3),
(@BASECENTRY+0, 5,0,"Coming for you now, puny mortals!",14,100,3),
(@BASECENTRY+0, 6,0,"%s charges at the barrier with renewed strength!",41,100,3),
(@BASECENTRY+0, 7,0,"Wrecker tired of games. Now, smash!",14,100,3),
(@BASECENTRY+0, 8,0,"%s begins to relentlessly pound the wall! Take it down!",41,100,3),
(@BASECENTRY+0, 9,0,"Hmph. Friend will not help you here.",14,100,3),
(@BASECENTRY+0,10,0,"Why... So tired....",12,100,3);

DELETE FROM `npc_text` WHERE `ID` between @BASETEXT+0 and @BASETEXT+1;
INSERT INTO `npc_text` (`ID`,`text0_0`,`text0_1`) VALUES
(@BASETEXT+0,"Hold, $c!$B$BJust beyond this gate stands an abomination of enormous size, ready to charge at my beleaguered companions that have retreated into the mountain beyond.$B$B$BFor now, it seems to have lost interest, but if you disturb its rest, it will certainly resume its attacks.$B$BI plead of you, $c. Do not enter this room.",""),
(@BASETEXT+1,"Halt, $c.$B$BJust beyond this gate, a brave adventurer is battling to defeat a peaceful being that would've harmed nobody if it hadn't been disturbed.$B$BI warned them not to enter, but they just wouldn't listen.$B$B<The watcher sighs heavily.>$B$BThey never listen.","");

DELETE FROM `gameobject_template` WHERE `entry` between @BASEOENTRY+0 and @BASEOENTRY+1;
INSERT INTO `gameobject_template` (`entry`,`type`,`displayId`,`name`,`flags`,`size`,`Data0`,`Data1`,`Data2`,`ScriptName`) VALUES
(@BASEOENTRY+0,0,4132,"boss_wrecker_gate01",4,1,0,0,30000,""),
(@BASEOENTRY+1,0,9214,"bonus_boss_icewall",4,2,1,0,0,"");
					
DELETE FROM `creature` WHERE `guid` between @BASECGUID+0 and @BASECGUID+1;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`movementtype`) VALUES
(@BASECGUID+0,@BASECENTRY+0,169,1,1,0,-145.9,2454.2,92.008,3.14159265359,30,0),
(@BASECGUID+1,@BASECENTRY+1,169,1,1,0,-94.71,2429.0,92.008,5.432591,     30,0);

DELETE FROM `gameobject` WHERE `guid` between @BASEOGUID+0 and @BASEOGUID+3;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`state`) VALUES
(@BASEOGUID+0,@BASEOENTRY+0,169,1,1,-110.434,2427.93,101.041,5.78446,0,0,0.246788,-0.969069,10,1),
(@BASEOGUID+1,@BASEOENTRY+0,169,1,1,-110.434,2427.93,92.0079,5.78446,0,0,0.246788,-0.969069,10,1),
(@BASEOGUID+2,@BASEOENTRY+1,169,1,1,-83.8265,2432.05,92.0079,4.43437,0,0,0.798265,-0.602306,10,1),
(@BASEOGUID+3,@BASEOENTRY+1,169,1,1,-105.475,2407.02,92.0079,1.1349 ,0,0,0.537481, 0.843276,10,1);
