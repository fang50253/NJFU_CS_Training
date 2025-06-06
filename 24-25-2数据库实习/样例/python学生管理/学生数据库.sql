SET FOREIGN_KEY_CHECKS=0;
 
-- ----------------------------
-- Table structure for `admin_login_k`
-- ----------------------------
DROP TABLE IF EXISTS `admin_login_k`;
CREATE TABLE `admin_login_k` (
  `admin_id` char(20) NOT NULL,
  `admin_pass` char(20) DEFAULT NULL,
  PRIMARY KEY (`admin_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
 
-- ----------------------------
-- Records of admin_login_k
-- ----------------------------
INSERT INTO `admin_login_k` VALUES ('admin', 'admin');
 
-- ----------------------------
-- Table structure for `student_k`
-- ----------------------------
DROP TABLE IF EXISTS `student_k`;
CREATE TABLE `student_k` (
  `id` char(20) NOT NULL,
  `name` char(20) DEFAULT NULL,
  `gender` char(5) DEFAULT NULL,
  `age` char(5) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
 
-- ----------------------------
-- Records of student_k
-- ----------------------------
INSERT INTO `student_k` VALUES ('209114122', '徐志强', '男', '21');
INSERT INTO `student_k` VALUES ('206546845', '张浩', '男', '21');
INSERT INTO `student_k` VALUES ('182085215', '汤姆', '男', '23');
INSERT INTO `student_k` VALUES ('182085211', '杰瑞', '男', '21');
INSERT INTO `student_k` VALUES ('182085212', '鲍勃', '男', '24');
INSERT INTO `student_k` VALUES ('182011011', '派大星', '男', '23');
INSERT INTO `student_k` VALUES ('209084163', '珊迪', '女', '29');
INSERT INTO `student_k` VALUES ('209045599', '章鱼哥', '男', '38');
INSERT INTO `student_k` VALUES ('209055599', '蟹阿金', '男', '42');
-- ----------------------------
-- Table structure for `stu_login_k`
-- ----------------------------
DROP TABLE IF EXISTS `stu_login_k`;
CREATE TABLE `stu_login_k` (
  `stu_id` char(20) NOT NULL,
  `stu_pass` char(20) DEFAULT NULL,
  PRIMARY KEY (`stu_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
 
-- ----------------------------
-- Records of stu_login_k
-- ----------------------------
INSERT INTO `stu_login_k` VALUES ('209114122', '123456');
INSERT INTO `stu_login_k` VALUES ('206546845', '123456');
INSERT INTO `stu_login_k` VALUES ('182085215', '123456');
INSERT INTO `stu_login_k` VALUES ('182085211', '123456');
INSERT INTO `stu_login_k` VALUES ('182085212', '123456');
INSERT INTO `stu_login_k` VALUES ('182011011', '123456');
INSERT INTO `stu_login_k` VALUES ('209084163', '123456');
INSERT INTO `stu_login_k` VALUES ('209045599', '123456');
INSERT INTO `stu_login_k` VALUES ('209055599', '123456');

 
-- ----------------------------
-- Table structure for `t_course`
-- ----------------------------
DROP TABLE IF EXISTS `t_course`;
CREATE TABLE `t_course` (
  `SNO` char(255) NOT NULL,
  `COURSE` char(255) DEFAULT NULL,
  `CREDIT` char(255) DEFAULT NULL,
  `GRADE` char(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
 
-- ----------------------------
-- Records of t_course
-- ----------------------------
INSERT INTO `t_course` VALUES ('08300205', '程序设计', '4', '88');
INSERT INTO `t_course` VALUES ('08300205', '数据库', '2.5', '90');
INSERT INTO `t_course` VALUES ('08300205', 'python', '5', '92');
INSERT INTO `t_course` VALUES ('08080929', '数据库', '2.5', '85');
INSERT INTO `t_course` VALUES ('09350124', '数据库', '2.5', '92');
INSERT INTO `t_course` VALUES ('09620233', '数据库', '2.5', '80');
INSERT INTO `t_course` VALUES ('09300218', '数据库', '2.5', '78');
INSERT INTO `t_course` VALUES ('09010122', '数据库', '2.5', '87');
INSERT INTO `t_course` VALUES ('08080929', '程序设计', '4', '86');
INSERT INTO `t_course` VALUES ('09010122', '程序设计', '4', '80');
INSERT INTO `t_course` VALUES ('08300516', '程序设计', '4', '76');
 
-- ----------------------------
-- Table structure for `t_st`
-- ----------------------------
DROP TABLE IF EXISTS `t_st`;
CREATE TABLE `t_st` (
  `SNO` char(11) NOT NULL,
  `SNAME` char(255) DEFAULT NULL,
  `SSEX` char(255) DEFAULT NULL,
  `AGE` char(255) DEFAULT NULL,
  `DEPT` char(255) DEFAULT NULL,
  PRIMARY KEY (`SNO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
 
-- ----------------------------
-- Records of t_st
-- ----------------------------
INSERT INTO `t_st` VALUES ('209044483', '王晨阳', '男', '20', '计204');
