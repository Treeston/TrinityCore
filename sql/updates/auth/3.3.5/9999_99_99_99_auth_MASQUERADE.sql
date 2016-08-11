-- Add required permissions
DELETE FROM `rbac_permissions` WHERE `id`=1942;
INSERT INTO `rbac_permissions` (`id`,`name`) VALUES (1942,"Command: .character masquerade");

DELETE FROM `rbac_linked_permissions` WHERE `linkedId`=1942;
INSERT INTO `rbac_linked_permissions` (`id`,`linkedId`) VALUES (196,1942);
