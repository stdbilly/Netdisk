-- MySQL dump 10.13  Distrib 8.0.16, for Linux (x86_64)
--
-- Host: localhost    Database: user
-- ------------------------------------------------------
-- Server version	8.0.16

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
 SET NAMES utf8mb4 ;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `file`
--

DROP TABLE IF EXISTS `file`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `file` (
  `dirID` int(11) NOT NULL AUTO_INCREMENT,
  `prevID` int(11) NOT NULL,
  `creator` varchar(50) DEFAULT NULL,
  `type` char(1) NOT NULL,
  `md5` varchar(40) DEFAULT NULL,
  `clientName` varchar(255) NOT NULL,
  `fileSize` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`dirID`),
  KEY `userName` (`creator`),
  KEY `md5` (`md5`),
  CONSTRAINT `file_ibfk_1` FOREIGN KEY (`creator`) REFERENCES `user` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=78 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `file`
--

LOCK TABLES `file` WRITE;
/*!40000 ALTER TABLE `file` DISABLE KEYS */;
INSERT INTO `file` VALUES (13,-1,'ffh','d',NULL,'/',4096),(14,-1,'ffh1','d',NULL,'/',4096),(15,-1,'ffh3','d',NULL,'/',4096),(16,-1,'ffh4','d',NULL,'/',4096),(17,-1,'ffh19','d',NULL,'/',4096),(18,-1,'ffh12','d',NULL,'/',4096),(23,18,'ffh','d',NULL,'ll',1),(73,18,NULL,'d',NULL,'hello',1),(74,18,NULL,'d',NULL,'hello2',1),(75,23,NULL,'d',NULL,'ll2',1),(76,23,NULL,'d',NULL,'ll3',1),(77,18,'ffh12','-','8a68e2d4a1761570bf35f2c5a6facee0','file',412679526);
/*!40000 ALTER TABLE `file` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `opLog`
--

DROP TABLE IF EXISTS `opLog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `opLog` (
  `userName` varchar(40) NOT NULL,
  `opTime` datetime DEFAULT CURRENT_TIMESTAMP,
  `op` varchar(200) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `opLog`
--

LOCK TABLES `opLog` WRITE;
/*!40000 ALTER TABLE `opLog` DISABLE KEYS */;
INSERT INTO `opLog` VALUES ('','2019-06-13 20:42:12','??? ffh12'),('ffh12','2019-06-13 20:42:15','ls '),('ffh12','2019-06-13 20:42:17','cd ll'),('ffh12','2019-06-13 20:42:19','cd ll2'),('ffh12','2019-06-13 20:42:21','cd ..'),('ffh12','2019-06-13 20:42:23','ls '),('ffh12','2019-06-13 20:42:31','ls '),('ffh12','2019-06-13 20:42:34','rm ll2'),('ffh12','2019-06-13 20:42:36','ls '),('ffh12','2019-06-13 20:42:37','cd ..'),('ffh12','2019-06-13 20:42:38','ls '),('ffh12','2019-06-13 20:42:39','cd ..'),('','2019-06-14 10:21:49','??? ffh12'),('ffh12','2019-06-14 10:21:51','ls '),('ffh12','2019-06-14 10:21:55','ls '),('ffh12','2019-06-14 10:22:02','ls '),('ffh12','2019-06-14 10:22:09','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-14 10:22:14','ls '),('','2019-06-14 10:22:44','??? ffh12'),('ffh12','2019-06-14 10:22:46','ls '),('ffh12','2019-06-14 10:22:50','gets file 0'),('ffh12','2019-06-14 10:22:53','ls '),('ffh12','2019-06-14 10:22:54','ls '),('ffh12','2019-06-14 10:23:00','ls '),('','2019-06-14 10:24:45','??? ffh12'),('ffh12','2019-06-14 10:24:46','ls '),('ffh12','2019-06-14 10:24:49','gets file 0'),('ffh12','2019-06-14 10:24:51','ls '),('ffh12','2019-06-14 10:24:53','ls '),('','2019-06-14 10:25:44','??? ffh12'),('ffh12','2019-06-14 10:25:45','ls '),('ffh12','2019-06-14 10:25:49','ls '),('ffh12','2019-06-14 10:25:52','gets file 0'),('ffh12','2019-06-14 10:25:55','ls '),('ffh12','2019-06-14 10:25:55','ls '),('','2019-06-14 18:41:45','login ffh12'),('ffh12','2019-06-14 18:41:47','ls '),('ffh12','2019-06-14 18:41:49','ls '),('ffh12','2019-06-14 18:41:53','gets '),('','2019-06-14 18:53:16','login ffh12'),('ffh12','2019-06-14 18:53:18','ls '),('ffh12','2019-06-14 18:53:21','gets '),('','2019-06-14 18:53:58','login ffh12'),('ffh12','2019-06-14 18:53:59','ls '),('ffh12','2019-06-14 18:54:02','gets '),('','2019-06-14 18:55:04','login ffh12'),('ffh12','2019-06-14 18:55:05','ls '),('ffh12','2019-06-14 18:55:07','gets '),('','2019-06-14 18:55:57','login ffh12'),('ffh12','2019-06-14 18:55:59','ls '),('ffh12','2019-06-14 18:56:00','gets '),('','2019-06-14 18:57:09','login ffh12'),('ffh12','2019-06-14 18:57:11','ls '),('ffh12','2019-06-14 18:57:14','gets '),('','2019-06-14 18:59:29','login ffh12'),('ffh12','2019-06-14 18:59:30','ls '),('ffh12','2019-06-14 18:59:33','gets '),('','2019-06-14 19:00:29','login ffh12'),('ffh12','2019-06-14 19:00:30','ls '),('ffh12','2019-06-14 19:00:32','gets '),('','2019-06-14 19:00:39','login ffh12'),('ffh12','2019-06-14 19:00:41','ls '),('ffh12','2019-06-14 19:00:44','gets '),('','2019-06-14 19:02:16','login ffh12'),('ffh12','2019-06-14 19:02:18','ls '),('ffh12','2019-06-14 19:02:20','pwd '),('ffh12','2019-06-14 19:02:22','gets '),('','2019-06-14 19:05:52','login ffh12'),('ffh12','2019-06-14 19:05:55','ls '),('ffh12','2019-06-14 19:05:58','gets '),('','2019-06-14 19:09:28','login ffh12'),('ffh12','2019-06-14 19:09:30','ls '),('ffh12','2019-06-14 19:09:32','gets '),('','2019-06-14 19:13:07','login ffh12'),('ffh12','2019-06-14 19:13:09','ls '),('ffh12','2019-06-14 19:13:12','gets '),('','2019-06-14 19:16:55','login ffh12'),('ffh12','2019-06-14 19:16:57','ls '),('ffh12','2019-06-14 19:16:59','gets '),('','2019-06-14 19:20:42','login ffh12'),('ffh12','2019-06-14 19:20:44','ls '),('ffh12','2019-06-14 19:20:46','gets file'),('','2019-06-14 19:23:59','login ffh12'),('ffh12','2019-06-14 19:24:00','ls '),('ffh12','2019-06-14 19:24:02','gets file'),('','2019-06-14 20:05:09','login ffh12'),('ffh12','2019-06-14 20:05:11','ls '),('ffh12','2019-06-14 20:05:14','gets file'),('','2019-06-14 20:07:54','login ffh12'),('ffh12','2019-06-14 20:07:55','ls '),('ffh12','2019-06-14 20:07:58','gets file'),('','2019-06-14 20:09:38','login ffh12'),('ffh12','2019-06-14 20:09:40','ls '),('ffh12','2019-06-14 20:09:42','gets file'),('','2019-06-14 20:12:30','login ffh12'),('ffh12','2019-06-14 20:12:31','ls '),('ffh12','2019-06-14 20:12:34','gets file'),('','2019-06-14 20:13:19','login ffh12'),('ffh12','2019-06-14 20:13:20','ls '),('ffh12','2019-06-14 20:13:22','gets file'),('','2019-06-14 20:14:32','login ffh12'),('ffh12','2019-06-14 20:14:34','ls '),('ffh12','2019-06-14 20:14:36','gets file'),('','2019-06-14 20:15:39','login ffh12'),('ffh12','2019-06-14 20:15:40','ls '),('ffh12','2019-06-14 20:15:42','gets file'),('','2019-06-14 20:17:06','login ffh12'),('ffh12','2019-06-14 20:17:07','ls '),('ffh12','2019-06-14 20:17:10','gets file'),('','2019-06-14 20:19:19','login ffh12'),('ffh12','2019-06-14 20:19:20','ls '),('ffh12','2019-06-14 20:19:22','gets file'),('','2019-06-14 20:20:41','login ffh12'),('ffh12','2019-06-14 20:20:42','ls '),('ffh12','2019-06-14 20:20:44','gets file'),('','2019-06-14 20:21:05','login ffh12'),('ffh12','2019-06-14 20:21:06','ls '),('ffh12','2019-06-14 20:21:09','gets file'),('','2019-06-14 20:22:25','login ffh12'),('ffh12','2019-06-14 20:22:26','ls '),('ffh12','2019-06-14 20:22:28','gets file'),('','2019-06-14 20:26:03','login ffh12'),('ffh12','2019-06-14 20:26:07','gets file'),('','2019-06-14 20:31:35','login ffh12'),('ffh12','2019-06-14 20:31:37','ls '),('ffh12','2019-06-14 20:31:40','gets file'),('','2019-06-14 20:32:18','login ffh12'),('ffh12','2019-06-14 20:32:23','gets file'),('','2019-06-14 20:33:21','login ffh12'),('ffh12','2019-06-14 20:33:22','ls '),('ffh12','2019-06-14 20:33:24','gets file'),('','2019-06-14 20:34:07','login ffh12'),('ffh12','2019-06-14 20:34:08','ls '),('ffh12','2019-06-14 20:34:10','gets file'),('','2019-06-14 20:37:38','login ffh12'),('ffh12','2019-06-14 20:37:40','ls '),('ffh12','2019-06-14 20:37:43','gets file'),('','2019-06-14 20:38:20','login ffh12'),('ffh12','2019-06-14 20:38:21','ls '),('ffh12','2019-06-14 20:38:23','gets file'),('','2019-06-14 20:38:58','login ffh12'),('ffh12','2019-06-14 20:38:59','ls '),('ffh12','2019-06-14 20:39:01','gets file'),('','2019-06-14 20:40:28','login ffh12'),('ffh12','2019-06-14 20:40:29','ls '),('ffh12','2019-06-14 20:40:31','gets file'),('','2019-06-14 20:44:36','login ffh12'),('ffh12','2019-06-14 20:44:38','ls '),('ffh12','2019-06-14 20:44:41','gets file'),('','2019-06-14 20:52:04','login ffh12'),('ffh12','2019-06-14 20:52:06','ls '),('ffh12','2019-06-14 20:52:08','gets file'),('','2019-06-14 20:55:01','login ffh12'),('ffh12','2019-06-14 20:55:02','ls '),('ffh12','2019-06-14 20:55:05','gets file'),('','2019-06-14 20:57:33','login ffh12'),('ffh12','2019-06-14 20:57:34','ls '),('ffh12','2019-06-14 20:57:36','gets file'),('','2019-06-14 21:21:23','login ffh12'),('ffh12','2019-06-14 21:21:25','ls '),('ffh12','2019-06-14 21:21:27','gets file'),('ffh12','2019-06-14 21:21:39','ls '),('ffh12','2019-06-14 21:21:42','cd ll'),('ffh12','2019-06-14 21:21:43','ls '),('ffh12','2019-06-14 21:21:45','cd ..'),('ffh12','2019-06-14 21:21:51','ls '),('','2019-06-14 21:52:35','login ffh12'),('ffh12','2019-06-14 21:52:37','ls '),('','2019-06-14 21:57:57','login ffh12'),('ffh12','2019-06-14 21:57:59','ls '),('ffh12','2019-06-14 21:58:00','cd ll'),('ffh12','2019-06-14 21:58:01','ls '),('ffh12','2019-06-14 21:58:03','cd ..'),('ffh12','2019-06-14 21:58:06','ls '),('ffh12','2019-06-14 21:58:08','cd file'),('ffh12','2019-06-14 21:58:08','ls '),('ffh12','2019-06-14 21:58:11','cd ..'),('ffh12','2019-06-14 21:58:12','ls '),('','2019-06-14 22:04:44','login ffh12'),('ffh12','2019-06-14 22:04:45','ls '),('ffh12','2019-06-14 22:04:56','pwd ll'),('ffh12','2019-06-14 22:05:00','ls '),('ffh12','2019-06-14 22:05:04','cd ll'),('','2019-06-14 23:26:23','login ffh12'),('ffh12','2019-06-14 23:26:24','ls '),('ffh12','2019-06-14 23:26:26','rm file'),('','2019-06-14 23:28:26','login ffh12'),('ffh12','2019-06-14 23:28:28','ls '),('ffh12','2019-06-14 23:28:41','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-14 23:28:45','ls '),('ffh12','2019-06-14 23:28:56','ls '),('ffh12','2019-06-14 23:28:57','ls '),('ffh12','2019-06-14 23:29:12','pwd '),('','2019-06-14 23:29:31','login ffh12'),('ffh12','2019-06-14 23:29:32','ls '),('ffh12','2019-06-14 23:29:41','puts d test.d '),('ffh12','2019-06-14 23:29:43','ls '),('','2019-06-14 23:30:22','login ls'),('','2019-06-14 23:30:24','login ls'),('','2019-06-14 23:30:28','login ls'),('','2019-06-14 23:30:31','login 3'),('','2019-06-14 23:31:57','login ffh12'),('ffh12','2019-06-14 23:32:00','ls '),('ffh12','2019-06-14 23:32:05','ls '),('','2019-06-15 09:20:12','??? ffh12'),('ffh12','2019-06-15 09:20:13','ls '),('ffh12','2019-06-15 09:20:16','pwd '),('ffh12','2019-06-15 09:20:19','ls '),('ffh12','2019-06-15 09:20:24','rm file'),('ffh12','2019-06-15 09:20:25','ls '),('ffh12','2019-06-15 09:20:30','ls '),('ffh12','2019-06-15 09:20:32','cd ll'),('ffh12','2019-06-15 09:20:34','pwd '),('ffh12','2019-06-15 09:20:37','ls '),('ffh12','2019-06-15 09:20:38','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:20:40','ls '),('ffh12','2019-06-15 09:20:42','ls '),('ffh12','2019-06-15 09:20:46','cd ..'),('ffh12','2019-06-15 09:20:47','ls '),('ffh12','2019-06-15 09:21:07','cd ..'),('ffh12','2019-06-15 09:21:08','ls '),('','2019-06-15 09:30:13','login ffh12'),('ffh12','2019-06-15 09:30:16','ls '),('ffh12','2019-06-15 09:30:27','rm window.o'),('ffh12','2019-06-15 09:30:28','ls '),('ffh12','2019-06-15 09:30:31','cd ll'),('ffh12','2019-06-15 09:30:32','ls '),('ffh12','2019-06-15 09:30:33','rm file'),('ffh12','2019-06-15 09:30:35','cd ..'),('ffh12','2019-06-15 09:30:36','ls '),('ffh12','2019-06-15 09:30:42','ls '),('ffh12','2019-06-15 09:30:43','pwd '),('ffh12','2019-06-15 09:30:45','cd ll'),('ffh12','2019-06-15 09:30:47','ls '),('ffh12','2019-06-15 09:30:48','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:30:51','ls '),('ffh12','2019-06-15 09:30:56','cd ..'),('ffh12','2019-06-15 09:30:56','ls '),('ffh12','2019-06-15 09:30:59','cd ll'),('ffh12','2019-06-15 09:31:00','ls '),('ffh12','2019-06-15 09:31:20','rm file'),('ffh12','2019-06-15 09:31:21','cd ..'),('ffh12','2019-06-15 09:31:22','ls '),('','2019-06-15 09:35:51','login 123'),('','2019-06-15 09:35:54','login ffh12'),('ffh12','2019-06-15 09:35:55','ls '),('ffh12','2019-06-15 09:36:02','ls '),('ffh12','2019-06-15 09:36:05','cd ll'),('ffh12','2019-06-15 09:36:06','ls '),('ffh12','2019-06-15 09:36:08','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:36:10','ls '),('ffh12','2019-06-15 09:36:13','cd ..'),('ffh12','2019-06-15 09:36:14','ls '),('ffh12','2019-06-15 09:36:16','cd ll'),('ffh12','2019-06-15 09:36:17','ls '),('ffh12','2019-06-15 09:36:34','cd ..'),('ffh12','2019-06-15 09:36:44','ls '),('ffh12','2019-06-15 09:36:45','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 09:37:30','login ffh12'),('ffh12','2019-06-15 09:37:32','ls '),('ffh12','2019-06-15 09:37:37','rm file'),('ffh12','2019-06-15 09:37:39','ls '),('ffh12','2019-06-15 09:37:46','ls '),('ffh12','2019-06-15 09:37:54','ls '),('','2019-06-15 09:38:10','login ffh12'),('ffh12','2019-06-15 09:38:12','ls '),('','2019-06-15 09:39:04','login ffh12'),('ffh12','2019-06-15 09:39:05','ls '),('ffh12','2019-06-15 09:39:16','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 09:39:29','login ffh12'),('ffh12','2019-06-15 09:39:30','ls '),('ffh12','2019-06-15 09:39:33','rm file'),('ffh12','2019-06-15 09:39:35','cd ll'),('ffh12','2019-06-15 09:39:35','ls '),('ffh12','2019-06-15 09:39:37','rm file'),('ffh12','2019-06-15 09:39:39','cd ..'),('ffh12','2019-06-15 09:39:39','ls '),('ffh12','2019-06-15 09:39:50','ls '),('ffh12','2019-06-15 09:39:51','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 09:40:36','login ffh12'),('ffh12','2019-06-15 09:40:38','ls '),('ffh12','2019-06-15 09:40:43','ls '),('ffh12','2019-06-15 09:40:47','pwd '),('ffh12','2019-06-15 09:40:50','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:40:52','ls '),('','2019-06-15 09:41:46','login ffh12'),('ffh12','2019-06-15 09:41:47','ls '),('ffh12','2019-06-15 09:41:49','cd ll'),('ffh12','2019-06-15 09:41:50','ls '),('ffh12','2019-06-15 09:41:52','cd ..'),('ffh12','2019-06-15 09:41:52','ls '),('ffh12','2019-06-15 09:41:58','ls '),('','2019-06-15 09:42:21','login ffh12'),('ffh12','2019-06-15 09:42:23','ls '),('ffh12','2019-06-15 09:42:28','ls '),('ffh12','2019-06-15 09:42:35','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:42:37','ls '),('ffh12','2019-06-15 09:42:39','ls '),('ffh12','2019-06-15 09:42:40','ls '),('ffh12','2019-06-15 09:42:41','ls '),('ffh12','2019-06-15 09:42:44','cd ll'),('ffh12','2019-06-15 09:42:45','ls '),('ffh12','2019-06-15 09:42:47','ls '),('ffh12','2019-06-15 09:42:49','cd ..'),('ffh12','2019-06-15 09:42:49','ls '),('','2019-06-15 09:44:08','??? ffh12'),('ffh12','2019-06-15 09:44:10','ls '),('','2019-06-15 09:45:00','??? ffh12'),('ffh12','2019-06-15 09:45:02','ls '),('ffh12','2019-06-15 09:45:09','ls '),('ffh12','2019-06-15 09:45:13','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:45:17','ls '),('','2019-06-15 09:45:49','??? ffh12'),('ffh12','2019-06-15 09:45:51','ls '),('ffh12','2019-06-15 09:45:53','rm file'),('ffh12','2019-06-15 09:46:04','ls '),('','2019-06-15 09:58:45','login ffh12'),('ffh12','2019-06-15 09:58:46','ls '),('ffh12','2019-06-15 09:58:58','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 09:59:07','ls '),('','2019-06-15 09:59:44','login ffh12'),('ffh12','2019-06-15 09:59:45','ls '),('ffh12','2019-06-15 09:59:49','gets file 0'),('ffh12','2019-06-15 09:59:52','ls '),('ffh12','2019-06-15 09:59:58','ls '),('','2019-06-15 10:00:16','login ffh12'),('ffh12','2019-06-15 10:00:17','ls '),('ffh12','2019-06-15 10:00:24','gets file 0'),('','2019-06-15 10:00:37','login ffh12'),('ffh12','2019-06-15 10:00:38','ls '),('ffh12','2019-06-15 10:00:40','gets file 189014159'),('ffh12','2019-06-15 10:00:43','ls '),('','2019-06-15 10:01:25','login ffh12'),('ffh12','2019-06-15 10:01:27','ls '),('ffh12','2019-06-15 10:01:32','rm file'),('ffh12','2019-06-15 10:01:33','ls '),('ffh12','2019-06-15 10:01:39','ls '),('ffh12','2019-06-15 10:01:46','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:02:10','login ffh12'),('ffh12','2019-06-15 10:02:13','ls '),('ffh12','2019-06-15 10:02:19','ls '),('ffh12','2019-06-15 10:02:25','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:06:33','login ffh12'),('ffh12','2019-06-15 10:06:35','ls '),('ffh12','2019-06-15 10:06:46','ls '),('ffh12','2019-06-15 10:06:46','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:06:55','login ffh12'),('ffh12','2019-06-15 10:06:57','ls '),('ffh12','2019-06-15 10:07:02','ls '),('ffh12','2019-06-15 10:07:04','pwd '),('ffh12','2019-06-15 10:07:10','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:08:47','login ffh12'),('ffh12','2019-06-15 10:08:48','ls '),('ffh12','2019-06-15 10:09:00','ls '),('ffh12','2019-06-15 10:09:00','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:09:19','login ffh12'),('ffh12','2019-06-15 10:09:20','ls '),('ffh12','2019-06-15 10:09:32','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 10:09:34','ls '),('ffh12','2019-06-15 10:09:36','ls '),('','2019-06-15 10:11:29','login ffh12'),('ffh12','2019-06-15 10:11:31','ls '),('ffh12','2019-06-15 10:11:43','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:11:52','login ffh12'),('ffh12','2019-06-15 10:12:05','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:12:27','login ffh12'),('ffh12','2019-06-15 10:12:39','ls '),('ffh12','2019-06-15 10:12:39','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 10:12:43','ls '),('','2019-06-15 10:15:59','login ffh12'),('ffh12','2019-06-15 10:16:01','ls '),('ffh12','2019-06-15 10:16:12','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:16:20','login ffh12'),('ffh12','2019-06-15 10:16:22','ls '),('ffh12','2019-06-15 10:16:35','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:18:20','login ffh12'),('ffh12','2019-06-15 10:18:22','ls '),('ffh12','2019-06-15 10:18:34','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:27:14','login ffh12'),('ffh12','2019-06-15 10:27:15','ls '),('ffh12','2019-06-15 10:27:18','rm file'),('ffh12','2019-06-15 10:27:32','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:28:04','login ffh12'),('ffh12','2019-06-15 10:28:17','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('ffh12','2019-06-15 10:28:21','ls '),('ffh12','2019-06-15 10:28:22','rm file'),('ffh12','2019-06-15 10:28:24','rm file'),('ffh12','2019-06-15 10:28:36','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:28:42','login ffh12'),('ffh12','2019-06-15 10:28:44','ls '),('ffh12','2019-06-15 10:28:55','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:30:18','login ffh12'),('ffh12','2019-06-15 10:30:20','ls '),('ffh12','2019-06-15 10:30:32','ls '),('ffh12','2019-06-15 10:30:32','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:31:39','login ffh12'),('ffh12','2019-06-15 10:31:40','ls '),('ffh12','2019-06-15 10:31:54','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:32:42','login ffh12'),('ffh12','2019-06-15 10:32:43','ls '),('','2019-06-15 10:33:08','login ffh12'),('ffh12','2019-06-15 10:33:09','ls '),('ffh12','2019-06-15 10:33:21','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 10:34:03','login ffh12'),('ffh12','2019-06-15 10:34:05','ls '),('ffh12','2019-06-15 10:34:08','rm file'),('','2019-06-15 10:34:21','login ffh12'),('ffh12','2019-06-15 10:34:22','ls '),('','2019-06-15 10:53:59','login ffh12'),('ffh12','2019-06-15 10:54:01','ls '),('ffh12','2019-06-15 10:54:03','cd ll'),('ffh12','2019-06-15 10:54:03','ls '),('ffh12','2019-06-15 10:54:05','cd ..'),('ffh12','2019-06-15 10:54:06','ls '),('','2019-06-15 11:01:46','login ffh12'),('ffh12','2019-06-15 11:01:59','puts - file 8a68e2d4a1761570bf35f2c5a6facee0'),('','2019-06-15 11:02:30','login ffh12'),('ffh12','2019-06-15 11:02:34','ls '),('ffh12','2019-06-15 11:02:37','gets file'),('','2019-06-15 11:04:14','login ffh12'),('ffh12','2019-06-15 11:04:15','ls '),('','2019-06-15 11:04:27','login ffh12'),('ffh12','2019-06-15 11:04:28','ls '),('ffh12','2019-06-15 11:04:32','gets file'),('ffh12','2019-06-15 11:04:37','ls ');
/*!40000 ALTER TABLE `opLog` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `upload`
--

DROP TABLE IF EXISTS `upload`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `upload` (
  `md5` varchar(40) NOT NULL,
  `fileSize` bigint(20) NOT NULL,
  `offset` bigint(20) NOT NULL,
  PRIMARY KEY (`md5`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `upload`
--

LOCK TABLES `upload` WRITE;
/*!40000 ALTER TABLE `upload` DISABLE KEYS */;
/*!40000 ALTER TABLE `upload` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(30) NOT NULL,
  `salt` varchar(50) DEFAULT NULL,
  `password` varchar(160) DEFAULT NULL,
  `dirID` int(11) DEFAULT NULL,
  `lastOPTime` bigint(20) DEFAULT NULL,
  `token` varchar(40) DEFAULT NULL,
  `currentDirId` int(11) DEFAULT NULL,
  `enrollTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `dirID` (`dirID`),
  CONSTRAINT `user_ibfk_1` FOREIGN KEY (`dirID`) REFERENCES `file` (`dirID`)
) ENGINE=InnoDB AUTO_INCREMENT=35 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (28,'ffh','$6$8w86p7e3','$6$8w86p7e3$.d32Ps/wfT4MA5pDse691GjCIkcU2ptTGYiNehzJTfV9GQCwnliI70FNF8mLMXL/..2m08AHt.SvKz5FajFM21',13,1560225283,NULL,NULL,'2019-06-13 12:05:05'),(29,'ffh1','$6$2eP2C5M4','$6$2eP2C5M4$CC133kwlIhmzMXGhHRRWG6R0KByq2G58CtFAuPhIsIzsL2iDZDZLG0unOscfQMhhSkt9EHP7LwjdsZLbmlJr51',14,1560260503,'0ebb061d515d53945fb91b66c2a8b997',NULL,'2019-06-13 12:05:05'),(30,'ffh3','$6$cv79KMRQ','$6$cv79KMRQ$OBaP6bcx9lJVMqE2IUlN/EVVdXP1SUVfgJmQ.ahAqANxMvhzJyfxrl4A7k6VajCRuEv9AD6WKMm8z42Gpv..K.',15,NULL,NULL,NULL,'2019-06-13 12:05:05'),(31,'ffh4','$6$4ByQsOEN','$6$4ByQsOEN$lv/SKT5sSX1SLMUFiIHHi/XsWAwjjoN733HV2ywWo.njN2Prq9uDFnKm9Rw9xEsPBSKqhnCVzU3OlcHSMBcYv1',16,NULL,NULL,NULL,'2019-06-13 12:05:05'),(32,'ffh19','$6$pkK1T3Xn','$6$pkK1T3Xn$qgcXQl3pBbXfX2flXPISw78ChV5VZ2CEExlnC143pX5ZI6Yv7ikoMBJQHPsjt8dJow/CjAcGjhEUIza5j11pc.',17,NULL,NULL,NULL,'2019-06-13 12:05:05'),(33,'ffh12','$6$RMy6ibbH','$6$RMy6ibbH$r0tOdJb8tfkJ/YqEq6Wemc04L6figHTSGtHbed8DZlt7icEQYTZ5WI.v1aWFf6WFAztKGcOuyNzqx.hoBbW6D.',18,1560567877,'b25203eeb2971d7c1448ef438a993cf1',18,'2019-06-13 12:05:05');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-07-11 10:15:23
