/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50546
Source Host           : localhost:3306
Source Database       : sm_world

Target Server Type    : MYSQL
Target Server Version : 50546
File Encoding         : 65001

Date: 2019-03-13 09:37:20
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `creature_vendor`
-- ----------------------------
DROP TABLE IF EXISTS `creature_vendor`;
CREATE TABLE `creature_vendor` (
  `guid` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `item` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `count` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `lastIncrementTime` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Npc System';

-- ----------------------------
-- Records of creature_vendor
-- ----------------------------
