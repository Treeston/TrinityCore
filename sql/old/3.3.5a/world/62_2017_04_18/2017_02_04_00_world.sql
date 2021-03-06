 -- East
DELETE FROM `script_spline_chain_meta` WHERE `entry`=39814 AND `chainId`=1;
INSERT INTO `script_spline_chain_meta` (`entry`,`chainId`,`splineId`,`expectedDuration`,`msUntilNext`) VALUES
(39814, 1, 0, 2314, 0);
DELETE FROM `script_spline_chain_waypoints` WHERE `entry`=39814 AND `chainId`=1;
INSERT INTO `script_spline_chain_waypoints` (`entry`,`chainId`,`splineId`,`wpId`,`x`,`y`,`z`) VALUES
(39814, 1, 0, 0, 3062.4450, 613.99300, 84.232710),
(39814, 1, 0, 1, 3058.9580, 610.40010, 85.552370),
(39814, 1, 0, 2, 3055.7080, 606.65010, 86.302370),
(39814, 1, 0, 3, 3046.2080, 596.15010, 86.552370),
(39814, 1, 0, 4, 3043.7080, 593.15010, 87.302370),
(39814, 1, 0, 5, 3040.2080, 589.65010, 88.052370),
(39814, 1, 0, 6, 3034.4580, 583.15010, 88.802370),
(39814, 1, 0, 7, 3014.9700, 561.80730, 88.872020);
 -- West
DELETE FROM `script_spline_chain_meta` WHERE `entry`=39814 AND `chainId`=2;
INSERT INTO `script_spline_chain_meta` (`entry`,`chainId`,`splineId`,`expectedDuration`,`msUntilNext`) VALUES
(39814, 2, 0, 3486, 0);
DELETE FROM `script_spline_chain_waypoints` WHERE `entry`=39814 AND `chainId`=2;
INSERT INTO `script_spline_chain_waypoints` (`entry`,`chainId`,`splineId`,`wpId`,`x`,`y`,`z`) VALUES
(39814, 2, 0, 0, 3047.4820, 454.40800, 84.374650),
(39814, 2, 0, 1, 3045.4070, 456.82900, 85.912670),
(39814, 2, 0, 2, 3043.6570, 458.57900, 86.912670),
(39814, 2, 0, 3, 3042.4070, 460.07900, 87.412670),
(39814, 2, 0, 4, 3041.6570, 460.57900, 87.662670),
(39814, 2, 0, 5, 3039.1570, 462.07900, 88.412670),
(39814, 2, 0, 6, 3037.9070, 462.82900, 88.662670),
(39814, 2, 0, 7, 3037.9070, 463.82900, 88.912670),
(39814, 2, 0, 8, 3037.9070, 464.82900, 89.162670),
(39814, 2, 0, 9, 3037.9070, 466.82900, 89.162670),
(39814, 2, 0, 10, 3033.4070, 469.07900, 89.412670),
(39814, 2, 0, 11, 3027.1570, 475.32900, 89.412670),
(39814, 2, 0, 12, 3025.1570, 477.32900, 89.412670),
(39814, 2, 0, 13, 3016.9070, 481.32900, 89.662670),
(39814, 2, 0, 14, 3014.9070, 483.32900, 89.662670),
(39814, 2, 0, 15, 3008.6570, 500.07900, 89.662670),
(39814, 2, 0, 16, 3003.8320, 501.25000, 89.450690);
