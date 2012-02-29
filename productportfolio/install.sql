-- phpMyAdmin SQL Dump
-- version 2.8.0.2
-- http://www.phpmyadmin.net
-- 
-- Host: nikita:3306
-- Generatie Tijd: 11 Aug 2006 om 12:12
-- Server versie: 3.23.58
-- PHP Versie: 5.1.2
-- 
-- Database: `guido_achievoproductportfolio`
-- 

-- --------------------------------------------------------

-- 
-- Tabel structuur voor tabel `productportfolio_product`
-- 

CREATE TABLE `productportfolio_product` (
  `id` int(10) NOT NULL default '0',
  `name` varchar(100) binary NOT NULL default '',
  `superproduct_id` int(10) default NULL,
  `description` text,
  `tags` varchar(100) binary default NULL,
  `usps` text,
  `conditions` text,
  `costsandparameters` text,
  `marketarea` int(1) default NULL,
  `uppselproduct` int(1) default NULL,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Tabel structuur voor tabel `productportfolio_product_targetgroup`
-- 

CREATE TABLE `productportfolio_product_targetgroup` (
  `product_id` int(10) NOT NULL default '0',
  `targetgroup_id` int(10) NOT NULL default '0',
  PRIMARY KEY  (`product_id`,`targetgroup_id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Tabel structuur voor tabel `productportfolio_representation`
-- 

CREATE TABLE `productportfolio_representation` (
  `id` int(10) NOT NULL default '0',
  `product_id` int(10) NOT NULL default '0',
  `name` varchar(100) binary NOT NULL default '',
  `description` text,
  `file_url` varchar(100) binary default NULL,
  `marketarea` int(1) default NULL,
  `product_url` varchar(100) binary default NULL,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Tabel structuur voor tabel `productportfolio_representation_targetgroup`
-- 

CREATE TABLE `productportfolio_representation_targetgroup` (
  `representation_id` int(10) NOT NULL default '0',
  `targetgroup_id` int(10) NOT NULL default '0',
  PRIMARY KEY  (`representation_id`,`targetgroup_id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Tabel structuur voor tabel `productportfolio_targetgroup`
-- 

CREATE TABLE `productportfolio_targetgroup` (
  `id` int(10) NOT NULL default '0',
  `name` varchar(100) binary NOT NULL default '',
  `comment` text,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;
