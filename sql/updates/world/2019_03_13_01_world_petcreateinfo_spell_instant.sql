/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50546
Source Host           : localhost:3306
Source Database       : sm_world

Target Server Type    : MYSQL
Target Server Version : 50546
File Encoding         : 65001

Date: 2019-03-13 09:38:21
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `petcreateinfo_spell_instant`
-- ----------------------------
DROP TABLE IF EXISTS `petcreateinfo_spell_instant`;
CREATE TABLE `petcreateinfo_spell_instant` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Spell1` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Spell2` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Spell3` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `Spell4` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Pet Create Spells';

-- ----------------------------
-- Records of petcreateinfo_spell_instant
-- ----------------------------
