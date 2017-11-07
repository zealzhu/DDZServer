/*
Navicat MySQL Data Transfer

Source Server         : .
Source Server Version : 50637
Source Host           : localhost:3306
Source Database       : game

Target Server Type    : MYSQL
Target Server Version : 50637
File Encoding         : 65001

Date: 2017-09-06 15:36:57
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_player
-- ----------------------------
DROP TABLE IF EXISTS `t_player`;
CREATE TABLE `t_player` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '唯一标识',
  `account` varchar(16) NOT NULL COMMENT '账号',
  `password` varchar(32) NOT NULL COMMENT '密码',
  `name` varchar(64) NOT NULL COMMENT '名字',
  `sex` enum('MALE','FEMALE') DEFAULT 'MALE' COMMENT '性别',
  `email` varchar(64) DEFAULT NULL COMMENT '邮箱',
  `mobile` varchar(11) DEFAULT NULL COMMENT '手机号',
  `phone` varchar(11) DEFAULT NULL COMMENT '座机号',
  `money` int(11) DEFAULT NULL COMMENT '金钱',
  `status` enum('ONLINE','OFFLINE','FORBIDDEN') DEFAULT 'OFFLINE' COMMENT '账号状态（ONLINE-在线;OFFLINE-离线;FORBIDDEN-禁用）',
  `desc` varchar(50) DEFAULT NULL COMMENT '描述',
  PRIMARY KEY (`id`),
  UNIQUE KEY `accountIndex` (`account`) USING HASH
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of t_player
-- ----------------------------
INSERT INTO `t_player` VALUES ('11', 'test', '123456', '测试', 'FEMALE', null, null, null, '10000', 'OFFLINE', null);
INSERT INTO `t_player` VALUES ('12', 'aaa', 'aaaaaa', '安安', 'FEMALE', null, null, null, '2222', 'OFFLINE', null);
INSERT INTO `t_player` VALUES ('13', 'zhu', '123456', '', 'MALE', 'test@nd.com', null, null, null, 'OFFLINE', null);
INSERT INTO `t_player` VALUES ('14', '5130761', '12345', '', 'MALE', null, null, null, null, 'OFFLINE', null);
INSERT INTO `t_player` VALUES ('15', '1', '1', '', 'MALE', null, null, null, null, 'OFFLINE', null);
INSERT INTO `t_player` VALUES ('16', '2', '2', '', 'MALE', null, null, null, null, 'OFFLINE', null);
INSERT INTO `t_player` VALUES ('17', '3', '3', '', 'MALE', null, null, null, null, 'OFFLINE', null);

-- ----------------------------
-- Table structure for t_play_log
-- ----------------------------
DROP TABLE IF EXISTS `t_play_log`;
CREATE TABLE `t_play_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) NOT NULL COMMENT '用户id',
  `score` double DEFAULT '0' COMMENT '本局得分',
  `multiple` int(11) DEFAULT '0' COMMENT '本局倍数',
  `time` datetime DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '游戏时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of t_play_log
-- ----------------------------
INSERT INTO `t_play_log` VALUES ('5', '11', '6400', '16', '2017-09-06 14:26:03');
INSERT INTO `t_play_log` VALUES ('6', '13', '-3200', '8', '2017-09-06 14:26:03');
INSERT INTO `t_play_log` VALUES ('7', '12', '-3200', '8', '2017-09-06 14:26:03');

-- ----------------------------
-- Table structure for t_score
-- ----------------------------
DROP TABLE IF EXISTS `t_score`;
CREATE TABLE `t_score` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) NOT NULL COMMENT '用户id',
  `score` float DEFAULT '0' COMMENT '得分',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of t_score
-- ----------------------------
INSERT INTO `t_score` VALUES ('6', '11', '8000');
INSERT INTO `t_score` VALUES ('7', '12', '-4000');
INSERT INTO `t_score` VALUES ('8', '13', '-4000');
SET FOREIGN_KEY_CHECKS=1;
