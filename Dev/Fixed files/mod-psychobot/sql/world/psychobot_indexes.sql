/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */
create index idx_gameobject_loot_template_item on gameobject_loot_template(item);
create index idx_disenchant_loot_template_item on disenchant_loot_template(item);
create index idx_fishing_loot_template_item on fishing_loot_template(item);
create index idx_item_loot_template_item on item_loot_template(item);
create index idx_pickpocketing_loot_template_item on pickpocketing_loot_template(item);
create index idx_reference_loot_template_item on reference_loot_template(item);
create index idx_skinning_loot_template_item on skinning_loot_template(item);
create index idx_creature_loot_template_item on creature_loot_template(item);