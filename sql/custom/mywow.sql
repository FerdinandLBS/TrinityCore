update world.creature_loot_template set Chance=100 where QuestRequired=1; -- 任务物品100%掉落
update world.item_template set stackable=200 where stackable=20;
update world.item_template set stackable=1000 where entry=6265; -- 灵魂碎片

delete from world.playercreateinfo_item;
insert into world.playercreateinfo_item (`race`, `class`, `itemid`, `amount`) VALUES ('0', '6', '40582', '-1');
INSERT INTO `world`.`playercreateinfo_item` (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '3914', '2');
INSERT INTO `world`.`playercreateinfo_item` (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '60000', '1'); -- tele bookplayercreateinfo_item
INSERT INTO `world`.`playercreateinfo_item` (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '38050', '1'); -- vendor
insert into world.playercreateinfo_item (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '11000', '1'); 
insert into world.playercreateinfo_item (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '11602', '1');
insert into world.playercreateinfo_item (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '28395', '1');
insert into world.playercreateinfo_item (`race`, `class`, `itemid`, `amount`) VALUES ('0', '0', '24490', '1');

-- 辅助单位
delete from world.spell_script_names where spell_id=85964;
INSERT INTO `world`.`spell_script_names` (`spell_id`, `ScriptName`) VALUES ('85964', 'spell_anetheron_vampiric_aura');

delete from world.creature_template where entry >= 45000;

	-- 西瓦多恩（防骑）
INSERT INTO `world`.`creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES ('45000', '1', '40598', '40597', '0', '18019');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45000', '0', '0', '0', '0', '0', '29604', '0', '0', '0', '西瓦多恩', '', '', '0', '1', '80', '2', '2070', '2', '1', '1.14286', '1', '0', '0', '1500', '2000', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '86000', '86002', '86004', '86001', '86003', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '1', '5', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- 兽人步兵
INSERT INTO `world`.`creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES ('45001', '1', '5289', '0', '2507', '18019');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45001', '0', '0', '0', '0', '0', '9794', '9795', '9797', '9796', '兽穴步兵', '', '', '0', '0', '80', '1', '85', '0', '1', '1.14286', '1', '0', '0', '2000', '1562', '1', '1', '1', '32768', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85990', '85991', '85993', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- Ms
INSERT INTO `world`.`creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES ('45002', '1', '1907', '0', '0', '18019');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45002', '0', '0', '0', '0', '0', '3344', '0', '0', '0', '女牧师安妮塔', '', '', '0', '0', '80', '0', '12', '2', '1', '1.14286', '1', '0', '0', '1500', '2000', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85980', '85982', '85981', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- 达拉然学徒
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45003', '0', '0', '0', '0', '0', '3767', '3768', '0', '0', '达拉然学徒', '', '', '0', '1', '80', '0', '76', '2', '1', '1.14286', '1', '0', '0', '2000', '2000', '1', '1', '8', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85970', '85971', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '1', '1', '1', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- 工程师 诺格
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45004', '0', '0', '0', '0', '0', '7135', '0', '0', '0', '诺格', '中级工程技师', '', '0', '1', '80', '0', '29', '2', '1', '1.14286', '1', '0', '0', '2000', '2000', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85960', '85962', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1.1', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- 罗宁
INSERT INTO `world`.`creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES ('45005', '1', '42139', '0', '0', '18019');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45005', '0', '0', '0', '0', '0', '16024', '0', '0', '0', '罗宁', '肯瑞托的老师', '', '0', '0', '80', '2', '2007', '2', '1', '1.14286', '0.7', '3', '0', '2000', '2000', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85970', '85976', '85972', '85973', '85974', '85971', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '3', '1', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- 萨鲁法尔
INSERT INTO `world`.`creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES ('45006', '1', '21580', '0', '0', '18019');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45006', '0', '0', '0', '0', '0', '23033', '0', '0', '0', '萨鲁法尔大王', '', '', '0', '0', '83', '2', '1979', '2', '1', '1.34286', '1', '1', '0', '1500', '2000', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85990', '85996', '85992', '85993', '85994', '85995', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '2', '1', '1', '1', '1', '0', '199', '1', '0', '0', '0', 'AssistanceAI', '12340');

    -- 微微
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('44000', '0', '0', '0', '0', '0', '857', '0', '0', '0', '小黑羊', '', '', '0', '1', '80', '0', '31', '2', '1.2', '1.24286', '1', '0', '0', '2000', '2000', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '87012', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '3', '0', '1', '1', '1', '0', '0', '1', '0', '0', '0', '', '12340');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45007', '0', '0', '0', '0', '0', '19193', '0', '0', '0', '微微', '牧羊姑娘', '', '0', '1', '80', '0', '1604', '2', '1', '1.14286', '1', '2', '0', '2000', '2000', '1', '1', '2', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85980', '85982', '85983', '85957', '85981', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '1', '1', '1', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', 'AssistanceAI', '12340');

   -- 恐惧魔王alter
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('44001', '0', '0', '0', '0', '0', '12817', '0', '0', '0', '炼狱火仆从', '', '', '0', '0', '80', '0', '90', '0', '1.23', '1.14286', '0.6', '0', '0', '1460', '1606', '1', '1', '1', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '29644', '12744', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', '', '12340');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45008', '0', '0', '0', '0', '0', '21069', '0', '0', '0', '安纳塞隆', '恐惧魔王', '', '0', '1', '80', '1', '1720', '2', '1.2', '1.14286', '0.22', '3', '0', '1200', '2000', '1', '1', '2', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85960', '85963', '85964', '86005', '0', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '1', '1', '1', '1', '0', '167', '1', '0', '0', '0', 'AssistanceAI', '12340');

  -- 黑暗游侠
INSERT INTO `world`.`creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES ('45009', '1', '2179', '0', '42775', '18019');
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('45009', '0', '0', '0', '0', '0', '28213', '0', '0', '0', '希尔瓦娜斯·风行者', '黑暗游侠', '', '0', '1', '80', '2', '118', '2', '1', '1.24286', '1', '3', '0', '2000', '2000', '1', '1', '2', '0', '2048', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85990', '59713', '20672', '59711', '59710', '0', '0', '0', '0', '0', '0', '0', 'AssistanceAI', '0', '1', '1', '0.5', '1', '1', '1', '0', '0', '1', '0', '0', '0', '', '12340');

-- 神器
-- 洒满图腾
delete from world.spell_script_names where spell_id=-89000;
delete from world.spell_proc where SpellId=-88004;
INSERT INTO `world`.`spell_script_names` (`spell_id`, `ScriptName`) VALUES ('-89000', 'spell_sha_lightning_overload');
INSERT INTO `world`.`spell_proc` (`SpellId`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `ProcFlags`, `SpellTypeMask`, `SpellPhaseMask`, `HitMask`, `AttributesMask`, `ProcsPerMinute`, `Chance`, `Cooldown`, `Charges`) VALUES ('-88004', '0', '11', '0', '16777232', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0');

UPDATE `world`.`item_template` SET `Quality`='5', `ItemLevel`='300', `spellid_1`='88010', `spelltrigger_1`='0', `spellid_2`='88006', `spelltrigger_2`='1', `spellid_3`='88002', `spelltrigger_3`='1', `spellid_4`='89000', `spelltrigger_4`='1', `spellid_5`='88004', `spelltrigger_5`='1', `description`='|CFFFF0000雷霆，听我召唤！\n|CFFEAAF00视同风之图腾、大地图腾、火焰图腾与水之图腾。\n' WHERE `entry`='46978';

-- 术士 任务达奥拉宝珠
UPDATE `world`.`item_template` SET `name`='欺诈者宝珠', `Quality`='5', `ItemLevel`='300', `stat_type1`='0', `stat_value1`='0', `spellid_1`='88005', `spellid_2`='88006', `spelltrigger_2`='1', `spellcooldown_2`='0', `spellid_3`='88008', `spelltrigger_3`='1',`description`='|CFFFF0000整个军团!都听我的号令！' WHERE `entry`='15108';
 
-- 战士
-- 饰品
INSERT INTO `world`.`item_template` (`entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `flagsCustom`, `VerifiedBuild`) VALUES ('60001', '4', '0', '0', '布洛克斯的勇气', '45597', '5', '0', '0', '1', '0', '0', '12', '1', '-1', '300', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '88005', '1', '0', '0', '-1', '0', '-1', '88012', '1', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '我能拿什么去面对诸界毁灭者？除了勇气，其他的都微不足道。', '0', '0', '0', '0', '0', '-1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '0', '0', '', '0', '0', '0', '0', '0', '12340');

-- 创建角色：武器技能
-- 单手剑
UPDATE `world`.`playercreateinfo_skills` SET `classMask`='47' WHERE `skill`='43';
-- 单手锤
UPDATE `world`.`playercreateinfo_skills` SET `classMask`='1115' WHERE `skill`='54';

-- test new spells
-- INSERT INTO `world`.`spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES ('116', '-205', '0', 'Test new frost arrow');


-- 为商人 添加 矿石
UPDATE `world`.`npc_vendor` SET `item`='40753' WHERE `entry`='27914' and`item`='38308' and`ExtendedCost`='2411';
UPDATE `world`.`npc_vendor` SET `item`='47241' WHERE `entry`='27914' and`item`='38291' and`ExtendedCost`='2408';
UPDATE `world`.`npc_vendor` SET `item`='49426' WHERE `entry`='27914' and`item`='38161' and`ExtendedCost`='2409';
UPDATE `world`.`creature_template` SET `flags_extra`='0' WHERE `entry`='27914';
delete from world.npc_vendor where entry=43500;
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '0', '2770');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '1', '2771');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '2', '2775');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '3', '2772');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '4', '2776');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '5', '7911');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '6', '3858');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '7', '10620');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '8', '23424');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '9', '23425');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '10', '23427');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '11', '23426');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '12', '36909');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '13', '36912');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '14', '36910');

-- 添加 金属锭
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '19', '2840');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '20', '3576');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '21', '2841');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '22', '2842');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '23', '3575');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '24', '3859');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '25', '3577');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '26', '6037');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '27', '3860');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '28', '12359');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '29', '12360');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '30', '23445');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '31', '23448');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '32', '23446');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '33', '23447');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '34', '23573');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '35', '23449');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '36', '36916');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '37', '36913');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '38', '41163');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '39', '37663');

-- 添加 石头
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '40', '2835');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '41', '2836');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '42', '2838');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '43', '7912');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '44', '12365');

-- 添加 草药
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '50', '2447');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '51', '765');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '52', '2449');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '53', '785');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '54', '2450');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '55', '2452');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '56', '2453');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '57', '3820');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '58', '3355');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '59', '3369');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '60', '3356');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '61', '3357');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '62', '3818');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '63', '3821');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '64', '3358');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '65', '4625');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '66', '8831');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '67', '8836');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '68', '8838');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '69', '8839');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '70', '8845');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '71', '8846');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '72', '13464');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '73', '13463');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '74', '13465');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '75', '13466');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '76', '13467');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '77', '13468');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '78', '22785');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '79', '22789');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '80', '22786');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '81', '22787');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '82', '22790');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '83', '22791');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '84', '22792');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '85', '22793');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '86', '22794');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '87', '36901');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '88', '37921');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '89', '36904');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '90', '36906');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '91', '36908');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '92', '36907');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '93', '36903');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '94', '36905');

-- 添加皮革
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '95', '2934');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '96', '2318');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '97', '783');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '98', '2319');
INSERT INTO world.npc_vendor (`entry`, `slot`, `item`) VALUES ('43500', '99', '4234');

delete from world.item_template where entry=60000;
INSERT INTO `world`.`item_template` (`entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `flagsCustom`, `VerifiedBuild`) VALUES ('60000', '15', '0', '-1', '穿越魔典', '6418', '5', '64', '0', '1', '0', '0', '0', '-1', '-1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '', '0', '0', '0', '0', '0', '-1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '0', '0', 'telebook_script', '0', '0', '0', '0', '0', '12340');
UPDATE `world`.`item_template` SET `spellid_1`='85002', `description`='从你一睁眼开始，这本充满力量的魔典就属于你。' WHERE `entry`='60000';

-- 随机属性
/*
delete from world.item_enchantment_template where entry=9001 or entry=9002 or entry=9003 or entry=9004;
-- 蓝色物品非武器系列
update world.item_template set RandomSuffix=9001 where Quality=3 and InventoryType>0 and RandomProperty=0 and InventoryType<>0 and InventoryType<>18 and InventoryType<>24 and InventoryType<>27 and InventoryType<>13 and InventoryType<>14 and InventoryType<>15 and InventoryType<>17 and InventoryType<>21 and InventoryType<>22;
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '117', '9.1');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '118', '9.1');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '119', '9.1');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '120', '9.1');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '121', '9.1');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '122', '9.1');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9001', '123', '9.1');

-- 蓝色武器
update world.item_template set RandomSuffix=9002 where Quality=3 and InventoryType>0 and RandomProperty=0 and InventoryType<>0 and InventoryType<>18 and InventoryType<>24 and InventoryType<>27 and (InventoryType=13 or InventoryType=14 or InventoryType=15 or InventoryType=17 or InventoryType=21 or InventoryType=22);
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '117', '9.83');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '118', '9.83');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '119', '9.83');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '120', '9.83');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '121', '9.83');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '122', '9.83');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9002', '123', '9.83');

-- zi色物品系列
	-- 非武器
update world.item_template set RandomSuffix=9003 where Quality=4 and InventoryType>0 and RandomProperty=0 and InventoryType<>0 and InventoryType<>18 and InventoryType<>24 and InventoryType<>27 and InventoryType<>13 and InventoryType<>14 and InventoryType<>15 and InventoryType<>17 and InventoryType<>21 and InventoryType<>22;
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '124', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '125', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '126', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '127', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '128', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '129', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9003', '130', '7.69');

-- 紫色武器
update world.item_template set RandomSuffix=9004 where Quality=4 and InventoryType>0 and RandomProperty=0 and InventoryType<>0 and InventoryType<>18 and InventoryType<>24 and InventoryType<>27 and (InventoryType=13 or InventoryType=14 or InventoryType=15 or InventoryType=17 or InventoryType=21 or InventoryType=22);
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '124', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '125', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '126', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '127', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '128', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '129', '7.69');
INSERT INTO `world`.`item_enchantment_template` (`entry`, `ench`, `chance`) VALUES ('9004', '130', '7.69');
*/

-- 新npc
-- 小黑杨
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('43500', '0', '0', '0', '0', '0', '857', '0', '0', '0', '小黑羊', '微微的守护者', '', '0', '80', '80', '0', '1665', '128', '1.2', '1.24286', '1.3', '0', '0', '2000', '2000', '1', '1', '1', '32768', '2048', '0', '0', '7', '2048', '0', '0', '0', '0', '0', '0', '0', '0', '0', '87012', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'SmartAI', '0', '1', '30', '0', '1', '1', '1', '0', '0', '1', '0', '0', '0', '', '12340');
-- 微微
INSERT INTO `world`.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES ('43501', '0', '0', '0', '0', '0', '19193', '0', '0', '0', '微微', '牧羊姑娘', '', '0', '80', '80', '0', '1665', '2', '1', '1.14286', '1', '1', '0', '2000', '2000', '1', '1', '2', '32768', '2048', '0', '0', '7', '2048', '0', '0', '0', '0', '0', '0', '0', '0', '0', '85980', '85982', '85983', '85957', '85981', '0', '0', '0', '0', '0', '0', '0', 'SmartAI', '1', '1', '30', '1', '1', '1', '1', '0', '0', '1', '0', '0', '0', '', '12340');



-- 职业橙任务

-- 1. 战士- 布洛克斯的勇气
INSERT INTO `world`.`creature_queststarter` (`id`, `quest`) VALUES ('43501', '27000');
INSERT INTO `world`.`creature_questender` (`id`, `quest`) VALUES ('43501', '27000');
INSERT INTO `world`.`quest_template` (`ID`, `QuestType`, `QuestLevel`, `MinLevel`, `QuestSortID`, `QuestInfoID`, `SuggestedGroupNum`, `RequiredFactionId1`, `RequiredFactionId2`, `RequiredFactionValue1`, `RequiredFactionValue2`, `RewardNextQuest`, `RewardXPDifficulty`, `RewardMoney`, `RewardBonusMoney`, `RewardDisplaySpell`, `RewardSpell`, `RewardHonor`, `RewardKillHonor`, `StartItem`, `Flags`, `RequiredPlayerKills`, `RewardItem1`, `RewardAmount1`, `RewardItem2`, `RewardAmount2`, `RewardItem3`, `RewardAmount3`, `RewardItem4`, `RewardAmount4`, `ItemDrop1`, `ItemDropQuantity1`, `ItemDrop2`, `ItemDropQuantity2`, `ItemDrop3`, `ItemDropQuantity3`, `ItemDrop4`, `ItemDropQuantity4`, `RewardChoiceItemID1`, `RewardChoiceItemQuantity1`, `RewardChoiceItemID2`, `RewardChoiceItemQuantity2`, `RewardChoiceItemID3`, `RewardChoiceItemQuantity3`, `RewardChoiceItemID4`, `RewardChoiceItemQuantity4`, `RewardChoiceItemID5`, `RewardChoiceItemQuantity5`, `RewardChoiceItemID6`, `RewardChoiceItemQuantity6`, `POIContinent`, `POIx`, `POIy`, `POIPriority`, `RewardTitle`, `RewardTalents`, `RewardArenaPoints`, `RewardFactionID1`, `RewardFactionValue1`, `RewardFactionOverride1`, `RewardFactionID2`, `RewardFactionValue2`, `RewardFactionOverride2`, `RewardFactionID3`, `RewardFactionValue3`, `RewardFactionOverride3`, `RewardFactionID4`, `RewardFactionValue4`, `RewardFactionOverride4`, `RewardFactionID5`, `RewardFactionValue5`, `RewardFactionOverride5`, `TimeAllowed`, `AllowableRaces`, `LogTitle`, `LogDescription`, `QuestDescription`, `AreaDescription`, `QuestCompletionLog`, `RequiredNpcOrGo1`, `RequiredNpcOrGo2`, `RequiredNpcOrGo3`, `RequiredNpcOrGo4`, `RequiredNpcOrGoCount1`, `RequiredNpcOrGoCount2`, `RequiredNpcOrGoCount3`, `RequiredNpcOrGoCount4`, `RequiredItemId1`, `RequiredItemId2`, `RequiredItemId3`, `RequiredItemId4`, `RequiredItemId5`, `RequiredItemId6`, `RequiredItemCount1`, `RequiredItemCount2`, `RequiredItemCount3`, `RequiredItemCount4`, `RequiredItemCount5`, `RequiredItemCount6`, `Unknown0`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `VerifiedBuild`) VALUES ('27000', '2', '82', '60', '0', '0', '0', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '200', '0', '60001', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '布洛克斯的勇气', '用勇气勋章证明你的勇气', '如果你能证明你的勇气，就有资格获得这个奖励', '', '去GM岛领取奖赏', '0', '0', '0', '0', '0', '0', '0', '0', '40753', '100', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '', '', '12340');



