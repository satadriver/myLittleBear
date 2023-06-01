/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50505
Source Host           : localhost:3306
Source Database       : computer

Target Server Type    : MYSQL
Target Server Version : 50505
File Encoding         : 65001

Date: 2019-06-13 10:49:58
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for pc_audio
-- ----------------------------
DROP TABLE IF EXISTS pc_audio;
CREATE TABLE pc_audio (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  size varchar(255) DEFAULT NULL COMMENT '录音大小',
  path varchar(255) DEFAULT NULL COMMENT '录音路径',
  time varchar(255) DEFAULT NULL COMMENT '时间',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_audio
-- ----------------------------
INSERT INTO pc_audio VALUES ('1', '1', '97.7B', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\1557413784.wav', '2019-05-08 19:06:38');
INSERT INTO pc_audio VALUES ('2', '1', '97.7B', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\1557409258.wav', '2019-05-08 19:06:37');

-- ----------------------------
-- Table structure for pc_client
-- ----------------------------
DROP TABLE IF EXISTS pc_client;
CREATE TABLE pc_client (
  id int(11) NOT NULL AUTO_INCREMENT,
  user_id int(11) DEFAULT NULL COMMENT '用户id',
  mac varchar(255) DEFAULT NULL COMMENT '客户端的唯一标识mac',
  name varchar(255) DEFAULT NULL COMMENT '客户端备注',
  info text COMMENT '客户端信息json格式',
  ts varchar(255) DEFAULT NULL COMMENT '客户端更新信息时间',
  state int(2) DEFAULT '0' COMMENT '在线状态\r\n1在线\r\n0离线',
  group_id int(11) DEFAULT '0' COMMENT '分组id',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_client
-- ----------------------------
INSERT INTO pc_client VALUES ('1', '2', '00_02_C9_54_E1_FC', '00_02_C9_54_E1_FC', '{\r\n	\"computerName\": \"G3P9DJ8AD0QQ037\",\r\n	\"userName\": \"Administrator\",\r\n	\"systemVersion\": \"Windows 10\",\r\n	\"Processor\": \"Intel(R) Core(TM) i7-4790K CPU @ 4.00GHz(64位8核)\",\r\n	\"phisicalMemory\": \"12230MB\",\r\n	\"diskSerialNO\": \"\",\r\n	\"diskModelNO\": \"\",\r\n	\"diskVolumeNO\": \"3258298895\",\r\n	\"MAC\": \"00 _02_C9_54_E1_FC \",\r\n	\"localIP\": \"192.168 .5 .106 \",\r\n	\"netIP\": \"您的IP是：[121.63 .65 .181] 来自： 湖北省宜昌市 电信 \",\r\n	\"gateWayIP\": \"192.168 .5 .1 \",\r\n	\"resolution\": \"1920 * 1080 \",\r\n	\"entrance\": \"C:\\\\Program Files(x86)\\\\Tencent\\\\QQ\\\\Bin\\\\QQ.exe\",\r\n	\"diskInfo\": \"磁盘总容量:3120(GB)C:\\\\盘容量: 120(GB),剩余容量: 51(GB)E: \\\\盘容量: 2000(GB),剩余容量: 1770(GB)F: \\\\盘容量: 314(GB),剩余容量: 248(GB)G: \\\\盘容量: 308(GB),剩余容量: 94(GB)H:\\\\盘容量: 314(GB),剩余容量: 301(GB)I:\\\\盘容量: 62(GB),剩余容量: 61(GB)\"\r\n}', '2019-06-07 19:03:36', '1', '1');
INSERT INTO pc_client VALUES ('2', '2', '00_02_C9_54_E1_F3', '', '{\r\n	\"computerName\": \"sss3P9DJ8AD0QQ037\",\r\n	\"userName\": \"Administrator\",\r\n	\"systemVersion\": \"Windows 10\",\r\n	\"Processor\": \"Intel(R) Core(TM) i7-4790K CPU @ 4.00GHz(64位8核)\",\r\n	\"phisicalMemory\": \"12230MB\",\r\n	\"diskSerialNO\": \"\",\r\n	\"diskModelNO\": \"\",\r\n	\"diskVolumeNO\": \"3258298895\",\r\n	\"MAC\": \"00 _02_C9_54_E1_FC \",\r\n	\"localIP\": \"192.168 .5 .106 \",\r\n	\"netIP\": \"您的IP是：[121.63 .65 .181] 来自： 湖北省宜昌市 电信 \",\r\n	\"gateWayIP\": \"192.168 .5 .1 \",\r\n	\"resolution\": \"1920 * 1080 \",\r\n	\"entrance\": \"C:\\\\Program Files(x86)\\\\Tencent\\\\QQ\\\\Bin\\\\QQ.exe\",\r\n	\"diskInfo\": \"磁盘总容量:3120(GB)C:\\\\盘容量: 120(GB),剩余容量: 51(GB)E: \\\\盘容量: 2000(GB),剩余容量: 1770(GB)F: \\\\盘容量: 314(GB),剩余容量: 248(GB)G: \\\\盘容量: 308(GB),剩余容量: 94(GB)H:\\\\盘容量: 314(GB),剩余容量: 301(GB)I:\\\\盘容量: 62(GB),剩余容量: 61(GB)\"\r\n}', '2019-06-07 19:03:36', '1', '3');

-- ----------------------------
-- Table structure for pc_downloadfile
-- ----------------------------
DROP TABLE IF EXISTS pc_downloadfile;
CREATE TABLE pc_downloadfile (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  file_path text,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_downloadfile
-- ----------------------------
INSERT INTO pc_downloadfile VALUES ('1', '1', 'D:\\$360Section\\360.1F8CDD1467E6C8040295126680B1AC87.q3q');
INSERT INTO pc_downloadfile VALUES ('2', '1', 'C:\\$360Section\\360.3C8A65220AB7B7F76F45EF94F09B01D2.q3q');
INSERT INTO pc_downloadfile VALUES ('3', '1', 'C:\\$360Section\\360.49CE6F7D761752C2F62BECF5F1CF8B6A.q3q');
INSERT INTO pc_downloadfile VALUES ('4', '1', 'C:\\$360Section\\360.4D15E3A005677A2DB5AA69B857642254.q3q');
INSERT INTO pc_downloadfile VALUES ('5', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I02VBOO');
INSERT INTO pc_downloadfile VALUES ('6', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I04VHZW');
INSERT INTO pc_downloadfile VALUES ('7', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I0EUHWZ');
INSERT INTO pc_downloadfile VALUES ('8', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I135UE2.lnk');
INSERT INTO pc_downloadfile VALUES ('9', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3KY4PF.exe');
INSERT INTO pc_downloadfile VALUES ('10', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3MQ8E0.lnk');
INSERT INTO pc_downloadfile VALUES ('11', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3V8BAE.ico');
INSERT INTO pc_downloadfile VALUES ('12', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I588W3R.ico');

-- ----------------------------
-- Table structure for pc_file
-- ----------------------------
DROP TABLE IF EXISTS pc_file;
CREATE TABLE pc_file (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  file_path varchar(255) DEFAULT NULL COMMENT '文件路径',
  PRIMARY KEY (id),
  KEY file (file_path)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_file
-- ----------------------------
INSERT INTO pc_file VALUES ('1', '1', 'C:\\$360Section\\360.1F8CDD1467E6C8040295126680B1AC87.q3q');
INSERT INTO pc_file VALUES ('2', '1', 'C:\\$360Section\\360.3C8A65220AB7B7F76F45EF94F09B01D2.q3q');
INSERT INTO pc_file VALUES ('3', '1', 'C:\\$360Section\\360.49CE6F7D761752C2F62BECF5F1CF8B6A.q3q');
INSERT INTO pc_file VALUES ('4', '1', 'C:\\$360Section\\360.4D15E3A005677A2DB5AA69B857642254.q3q');
INSERT INTO pc_file VALUES ('5', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I02VBOO');
INSERT INTO pc_file VALUES ('6', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I04VHZW');
INSERT INTO pc_file VALUES ('7', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I0EUHWZ');
INSERT INTO pc_file VALUES ('8', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I135UE2.lnk');
INSERT INTO pc_file VALUES ('9', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3KY4PF.exe');
INSERT INTO pc_file VALUES ('10', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3MQ8E0.lnk');
INSERT INTO pc_file VALUES ('11', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3V8BAE.ico');
INSERT INTO pc_file VALUES ('12', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I588W3R.ico');

-- ----------------------------
-- Table structure for pc_filemonitor
-- ----------------------------
DROP TABLE IF EXISTS pc_filemonitor;
CREATE TABLE pc_filemonitor (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  content varchar(255) DEFAULT NULL COMMENT '文件操作记录',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_filemonitor
-- ----------------------------
INSERT INTO pc_filemonitor VALUES ('1', '1', 'sdadsafdasfd');

-- ----------------------------
-- Table structure for pc_group
-- ----------------------------
DROP TABLE IF EXISTS pc_group;
CREATE TABLE pc_group (
  id int(11) NOT NULL AUTO_INCREMENT,
  user_id int(11) DEFAULT NULL COMMENT '用户id',
  group_name varchar(255) DEFAULT NULL COMMENT '分组名称',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_group
-- ----------------------------
INSERT INTO pc_group VALUES ('1', '2', '测试');
INSERT INTO pc_group VALUES ('3', '2', 'fdas');

-- ----------------------------
-- Table structure for pc_install
-- ----------------------------
DROP TABLE IF EXISTS pc_install;
CREATE TABLE pc_install (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  soft_name varchar(255) DEFAULT NULL COMMENT '软件名称',
  version varchar(255) DEFAULT NULL COMMENT '版本',
  install_path varchar(255) DEFAULT NULL COMMENT '安装路径',
  publisher varchar(255) DEFAULT NULL COMMENT '发布方',
  uninstall_path varchar(255) DEFAULT NULL COMMENT '下载路径',
  time varchar(255) DEFAULT NULL COMMENT '时间',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_install
-- ----------------------------
INSERT INTO pc_install VALUES ('1', '1', '安全卫士 - 2345', 'v3.9', 'C:\\Program Files (x86)\\Common Files\\Microsoft Shared\\OFFICE14\\Office Setup Controller\\setup.exe /uninstall PROPLUS /dll OSETUP.DLL', '上海展盟网络科技有限公司', 'C:\\Program Files (x86)\\Common Files\\Microsoft Shared\\OFFICE14\\Office Setup Controller\\setup.exe /uninstall PROPLUS /dll OSETUP.DLL', '2019-6-8 14:45:27');

-- ----------------------------
-- Table structure for pc_keyboard
-- ----------------------------
DROP TABLE IF EXISTS pc_keyboard;
CREATE TABLE pc_keyboard (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  content text,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS pc_clipboard;
CREATE TABLE pc_clipboard (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  content text,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_keyboard
-- ----------------------------
INSERT INTO pc_keyboard VALUES ('1', '1', 'ffadsfdsa');
INSERT INTO pc_keyboard VALUES ('2', '1', '21321');

-- ----------------------------
-- Table structure for pc_logs
-- ----------------------------
DROP TABLE IF EXISTS pc_logs;
CREATE TABLE pc_logs (
  id int(11) NOT NULL AUTO_INCREMENT,
  userid int(11) DEFAULT NULL,
  info varchar(255) DEFAULT NULL,
  time varchar(255) DEFAULT NULL,
  ip varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=46 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_logs
-- ----------------------------
INSERT INTO pc_logs VALUES ('34', '1', '登陆系统', '2019-06-07 17:53:43', '127.0.0.1');
INSERT INTO pc_logs VALUES ('35', '1', '登陆系统', '2019-06-07 18:23:22', '127.0.0.1');
INSERT INTO pc_logs VALUES ('36', '2', '登陆系统', '2019-06-07 18:26:26', '127.0.0.1');
INSERT INTO pc_logs VALUES ('37', '1', '登陆系统', '2019-06-07 18:36:19', '127.0.0.1');
INSERT INTO pc_logs VALUES ('38', '2', '登陆系统', '2019-06-07 18:36:42', '127.0.0.1');
INSERT INTO pc_logs VALUES ('39', '2', '登陆系统', '2019-06-08 11:12:20', '127.0.0.1');
INSERT INTO pc_logs VALUES ('40', '2', '登陆系统', '2019-06-08 17:38:49', '39.78.86.56');
INSERT INTO pc_logs VALUES ('41', '2', '登陆系统', '2019-06-08 17:42:27', '39.78.86.56');
INSERT INTO pc_logs VALUES ('42', '2', '登陆系统', '2019-06-08 17:42:27', '39.78.86.56');
INSERT INTO pc_logs VALUES ('43', '2', '登陆系统', '2019-06-08 17:42:28', '39.78.86.56');
INSERT INTO pc_logs VALUES ('44', '2', '登陆系统', '2019-06-08 17:44:32', '39.78.86.56');
INSERT INTO pc_logs VALUES ('45', '2', '登陆系统', '2019-06-08 20:26:08', '36.17.92.124');

-- ----------------------------
-- Table structure for pc_onlinelog
-- ----------------------------
DROP TABLE IF EXISTS pc_onlinelog;
CREATE TABLE pc_onlinelog (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id varchar(255) DEFAULT NULL COMMENT '客户端id',
  type varchar(255) DEFAULT NULL COMMENT '类型',
  time varchar(255) DEFAULT NULL COMMENT '时间',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_onlinelog
-- ----------------------------
INSERT INTO pc_onlinelog VALUES ('1', '1', '上线', '2019-6-8 15:17:30');

-- ----------------------------
-- Table structure for pc_photo
-- ----------------------------
DROP TABLE IF EXISTS pc_photo;
CREATE TABLE pc_photo (
  id int(11) NOT NULL AUTO_INCREMENT,
  type int(10) DEFAULT NULL COMMENT '分类1摄像头拍照2应用截屏3屏幕截屏',
  path varchar(255) DEFAULT NULL COMMENT '图片路径',
  time varchar(255) DEFAULT NULL COMMENT '时间',
  client_id int(11) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_photo
-- ----------------------------
INSERT INTO pc_photo VALUES ('1', '1', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\app_screen_2019_6_5_16_22_47.jpg', '2019-6-8 11:52:17', '1');
INSERT INTO pc_photo VALUES ('2', '1', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\app_screen_2019_6_5_17_33_19.jpg', '2019-6-8 11:53:32', '1');
INSERT INTO pc_photo VALUES ('3', '2', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\app_screen_2019_6_5_17_33_19.jpg', '2019-6-8 11:53:32', '1');
INSERT INTO pc_photo VALUES ('4', '3', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\app_screen_2019_6_5_17_33_19.jpg', '2019-6-8 11:53:32', '1');
INSERT INTO pc_photo VALUES ('5', '1', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\app_screen_2019_6_5_17_33_19.jpg', '2019-6-8 11:53:32', '1');
INSERT INTO pc_photo VALUES ('6', '1', 'E:\\PHP_project\\computer\\server\\00_02_C9_54_E1_FC\\app_screen_2019_6_5_17_33_19.jpg', '2019-6-8 11:53:32', '1');

-- ----------------------------
-- Table structure for pc_process
-- ----------------------------
DROP TABLE IF EXISTS pc_process;
CREATE TABLE pc_process (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  process_id int(11) DEFAULT NULL COMMENT '进程id',
  process_name varchar(255) DEFAULT NULL COMMENT '进程名称',
  application_name varchar(255) DEFAULT NULL COMMENT '应用名称',
  install_path varchar(255) DEFAULT NULL COMMENT '安装路径',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_process
-- ----------------------------
INSERT INTO pc_process VALUES ('1', '1', '4', 'wininit.exe', '百度网盘', 'C:\\Program Files (x86)\\Common Files\\Microsoft Shared\\OFFICE14\\Office Setup Controller\\setup.exe /uninstall PROPLUS /dll OSETUP.DLL');

-- ----------------------------
-- Table structure for pc_shell
-- ----------------------------
DROP TABLE IF EXISTS pc_shell;
CREATE TABLE pc_shell (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  content varchar(255) DEFAULT NULL COMMENT '内容',
  time varchar(255) DEFAULT NULL COMMENT '时间',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_shell
-- ----------------------------
INSERT INTO pc_shell VALUES ('1', '1', 'SetBootAutoRun lpSetFileAttributesA logfile error code:2', '2019-6-8 15:13:20');

-- ----------------------------
-- Table structure for pc_uploadfile
-- ----------------------------
DROP TABLE IF EXISTS pc_uploadfile;
CREATE TABLE pc_uploadfile (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  file_path text COMMENT '文件路径',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_uploadfile
-- ----------------------------
INSERT INTO pc_uploadfile VALUES ('1', '1', 'E:\\$360Section\\360.1F8CDD1467E6C8040295126680B1AC87.q3q');
INSERT INTO pc_uploadfile VALUES ('2', '1', 'C:\\$360Section\\360.3C8A65220AB7B7F76F45EF94F09B01D2.q3q');
INSERT INTO pc_uploadfile VALUES ('3', '1', 'C:\\$360Section\\360.49CE6F7D761752C2F62BECF5F1CF8B6A.q3q');
INSERT INTO pc_uploadfile VALUES ('4', '1', 'C:\\$360Section\\360.4D15E3A005677A2DB5AA69B857642254.q3q');
INSERT INTO pc_uploadfile VALUES ('5', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I02VBOO');
INSERT INTO pc_uploadfile VALUES ('6', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I04VHZW');
INSERT INTO pc_uploadfile VALUES ('7', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I0EUHWZ');
INSERT INTO pc_uploadfile VALUES ('8', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I135UE2.lnk');
INSERT INTO pc_uploadfile VALUES ('9', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3KY4PF.exe');
INSERT INTO pc_uploadfile VALUES ('10', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3MQ8E0.lnk');
INSERT INTO pc_uploadfile VALUES ('11', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I3V8BAE.ico');
INSERT INTO pc_uploadfile VALUES ('12', '1', 'C:\\$Recycle.Bin\\S-1-5-21-1834244928-108800066-2192006236-1001\\$I588W3R.ico');

-- ----------------------------
-- Table structure for pc_user
-- ----------------------------
DROP TABLE IF EXISTS pc_user;
CREATE TABLE pc_user (
  id int(11) NOT NULL AUTO_INCREMENT,
  user varchar(255) DEFAULT NULL,
  password varchar(255) DEFAULT NULL,
  state int(2) DEFAULT '1' COMMENT '0停用1正常',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=35 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of pc_user
-- ----------------------------
INSERT INTO pc_user VALUES ('2', 'test20190425', 'e10adc3949ba59abbe56e057f20f883e', '1');
INSERT INTO pc_user VALUES ('1', 'admin', 'e10adc3949ba59abbe56e057f20f883e', '1');
