ALTER TABLE `item_instance`
    ADD COLUMN `data` longtext, AFTER `itemTextId`;
	
ALTER TABLE `characters`
    ADD COLUMN `characterMode` tinyint(1) unsigned NOT NULL DEFAULT '0',, AFTER `activeSpec`;