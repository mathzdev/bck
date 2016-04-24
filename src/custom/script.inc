// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Script Commands
 * Place the body of custom script commands in this file.
 * Format:
 *	BUILDIN_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/
/**
 * adopt("<parent_name>","<baby_name>");
 * adopt(<parent_id>,<baby_id>);
 * https://rathena.org/board/topic/104014-suggestion-add-adopt-or-etc/
 */
BUILDIN_FUNC(unitexists)
{
	struct block_list* bl;

	bl = map_id2bl(script_getnum(st, 2));

	if (!bl)
		script_pushint(st, false);
	else
		script_pushint(st, true);

	return SCRIPT_CMD_SUCCESS;
}

/**
 * navigateto("<map>"{,<x>,<y>,<flag>,<hide_window>,<monster_id>,<char_id>});
 */
BUILDIN_FUNC(navigateto){
#if PACKETVER >= 20111010
	TBL_PC* sd;
	const char *map_c;
	uint16 x = 0, y = 0, monster_id = 0;
	uint8 flag = NAV_KAFRA_AND_PLANE;
	bool hideWindow = true;

	map_c = script_getstr(st,2);

	if( script_hasdata(st,3) )
		x = script_getnum(st,3);
	if( script_hasdata(st,4) )
		y = script_getnum(st,4);
	if( script_hasdata(st,5) )
		flag = (uint8)script_getnum(st,5);
	if( script_hasdata(st,6) )
		hideWindow = script_getnum(st,6) ? true : false;
	if( script_hasdata(st,7) )
		monster_id = script_getnum(st,7);

	if (!script_charid2sd(8, sd))
		return SCRIPT_CMD_FAILURE;

	clif_navigateTo(sd,map_c,x,y,flag,hideWindow,monster_id);

		return SCRIPT_CMD_SUCCESS;
#else
	return SCRIPT_CMD_FAILURE;
#endif
}

BUILDIN_FUNC(adopt)
{
	TBL_PC *sd, *b_sd;
	struct script_data *data;
	enum adopt_responses response;

	data = script_getdata(st, 2);
	get_val(st, data);

	if (data_isstring(data)) {
		const char *name = conv_str(st, data);

		sd = map_nick2sd(name);
		if (sd == NULL) {
			ShowError("buildin_adopt: Non-existant parent character %s requested.\n", name);
			return SCRIPT_CMD_FAILURE;
		}
	} else if (data_isint(data)) {
		uint32 char_id = conv_num(st, data);

		sd = map_charid2sd(char_id);
		if (sd == NULL) {
			ShowError("buildin_adopt: Non-existant parent character %d requested.\n", char_id);
			return SCRIPT_CMD_FAILURE;
		}
	} else {
		ShowError("buildin_adopt: Invalid data type for argument #1 (%d).", data->type);
		return SCRIPT_CMD_FAILURE;
	}

	data = script_getdata(st, 3);
	get_val(st, data);

	if (data_isstring(data)) {
		const char *name = conv_str(st, data);

		b_sd = map_nick2sd(name);
		if (b_sd == NULL) {
			ShowError("buildin_adopt: Non-existant baby character %s requested.\n", name);
			return SCRIPT_CMD_FAILURE;
		}
	} else if (data_isint(data)) {
		uint32 char_id = conv_num(st, data);

		b_sd = map_charid2sd(char_id);
		if (b_sd == NULL) {
			ShowError("buildin_adopt: Non-existant baby character %d requested.\n", char_id);
			return SCRIPT_CMD_FAILURE;
		}
	} else {
		ShowError("buildin_adopt: Invalid data type for argument #2 (%d).", data->type);
		return SCRIPT_CMD_FAILURE;
	}

	response = pc_try_adopt(sd, map_charid2sd(sd->status.partner_id), b_sd);

	if (response == ADOPT_ALLOWED) {
		TBL_PC *p_sd = map_charid2sd(sd->status.partner_id);

		b_sd->adopt_invite = sd->status.account_id;
		clif_Adopt_request(b_sd, sd, p_sd->status.account_id);
		script_pushint(st, ADOPT_ALLOWED);
		return SCRIPT_CMD_SUCCESS;
	}

	script_pushint(st, response);
	return SCRIPT_CMD_FAILURE;
}
// Achievement System
BUILDIN_FUNC(achieve)
{
	struct map_session_data* sd = script_rid2sd(st);
	int id = script_getnum(st,2);
	struct achievement_data *ad;

	if( (ad = achievement_search(id)) != NULL )
		achievement_complete(sd,ad);

	return 0;
}

BUILDIN_FUNC(achievement_info)
{
	struct achievement_data *ad;
	int id, flag;

	id = script_getnum(st,2);
	flag = script_getnum(st,3);

	if( (ad = achievement_search(id)) == NULL )
	{
		ShowError("buildin:achievement_info: No achievement found with id %d.\n",id);
		return 0;
	}

	switch( flag )
	{
	case 0: // Status
		{
			struct map_session_data* sd = script_rid2sd(st);
			int index;
			if( !sd )
			{
				ShowError("buildin:achievement_info: No player attached.\n");
				return 0;
			}
			index = achievement_index(sd,id);
			script_pushint(st,(index >= 0 && sd->achievement[index].completed) ? 1 : 0);
		}
		break;
	case 1: // Name
		script_pushconststr(st,ad->name);
		break;
	case 2: // Cut-in
		script_pushconststr(st,ad->cutin);
		break;
	case 3: // Reward Base Exp
		script_pushint(st,ad->bexp);
		break;
	case 4: // Reward Base Exp
		script_pushint(st,ad->jexp);
		break;
	case 5: // Reward Item ID
		script_pushint(st,ad->nameid);
		break;
	}

	return 0;
}

/*==========================================
 * Cdigo Zephyrus para mob con IDdeParty para Map Dominacion
 *------------------------------------------*/
BUILDIN_FUNC(mobevent)
{
	int class_,size,amount,x,y,partyid,mostrarhp,hpmas,allied,noslaves,noexpnodrop,iswar,exp_boost,drop_boost;
	int item_drop, item_amount;
	const char *str,*mapm,*event="";

	mapm	        = script_getstr(st,2);
	x	        = script_getnum(st,3);
	y	        = script_getnum(st,4);
	str	        = script_getstr(st,5);
	class_      = script_getnum(st,6);
	size        = script_getnum(st,7);
	amount      = script_getnum(st,8);
	partyid     = script_getnum(st,9);
	mostrarhp   = script_getnum(st,10);
	hpmas       = script_getnum(st,11);
	allied      = script_getnum(st,12);
	noslaves    = script_getnum(st,13);
	noexpnodrop = script_getnum(st,14);
	item_drop   = script_getnum(st,15);
	item_amount = script_getnum(st,16);
	iswar       = script_getnum(st,17);
	exp_boost   = script_getnum(st,18);
	drop_boost  = script_getnum(st,19);

	if( script_hasdata(st,20) )
	{
		event = script_getstr(st,20);
		check_event(st, event);
	}

	if( class_ >= 0 && !mobdb_checkid(class_) )
	{
		ShowWarning("buildin_monster: Attempted to spawn non-existing monster class %d\n", class_);
		return 1;
	}
	mob_once_spawn_especial(map_id2sd(st->rid),mapm,x,y,str,class_,amount,event,hpmas,size,allied,noslaves,0,mostrarhp,0,0,noexpnodrop,partyid,item_drop,item_amount,iswar,exp_boost,drop_boost);
	script_pushint(st,1); // Confirmando creacion

	return 0;
}
/*==========================================
 * Get a Random mob ID
 *------------------------------------------*/
BUILDIN_FUNC(getmobrandid)
{
	int level = script_getnum(st,2);
	int range = script_getnum(st,3);

	script_pushint(st, mob_get_random_id_lv(level, range));
	return 0;
}
/*==========================================
 * Item Security System
 *------------------------------------------*/
BUILDIN_FUNC(setsecurity)
{
	struct map_session_data *sd = script_rid2sd(st);
	int value = script_getnum(st,2);
	if( sd == NULL )
		return 0;

	sd->state.secure_items = (value)?1:0;
	return 0;
}

BUILDIN_FUNC(getsecurity)
{
	struct map_session_data *sd = script_rid2sd(st);
	if( sd == NULL )
		return 0;

	script_pushint(st,sd->state.secure_items);
	return 0;
}
/*==========================================
 * Hunting Missions [Zephyrus]
 *------------------------------------------*/
BUILDIN_FUNC(mission_sethunting)
{
	struct map_session_data *sd = script_rid2sd(st);
	int index, i;
	char varname[32];

	if( sd == NULL )
		return 0;

	index = script_getnum(st,2);
	if( index < 1 || index > 5 )
		return 0; // Invalid Index

	i = index - 1;

	sprintf(varname, "Mission_ID%d", index);
	sd->hunting[i].mob_id = script_getnum(st,3);
	pc_setglobalreg(sd, add_str(varname), sd->hunting[i].mob_id);

	sprintf(varname, "Mission_Count%d", index);
	sd->hunting[i].count = script_getnum(st,4);
	pc_setglobalreg(sd, add_str(varname), sd->hunting[i].count);

	return 0;
}

BUILDIN_FUNC(mission_settime)
{
	struct map_session_data *sd = script_rid2sd(st);

	if( sd == NULL )
		return 0;

	sd->hunting_time = script_getnum(st,2);
	pc_setglobalreg(sd, add_str("Mission_Tick"), sd->hunting_time);

	return 0;
}

/*==========================================
 * Refine +1 item at pos and log and display refine
 *------------------------------------------*/
BUILDIN_FUNC(costume)
{
	int i = -1, num, ep;
	TBL_PC *sd;

	num = script_getnum(st,2); // Equip Slot
	sd = script_rid2sd(st);

	if( sd == NULL )
		return 0;
	if( num > 0 && num <= ARRAYLENGTH(equip) )
		i = pc_checkequip(sd, equip[num - 1]);
	if( i < 0 )
		return 0;

	ep = sd->status.inventory[i].equip;
	if( !(ep&EQP_HEAD_LOW) && !(ep&EQP_HEAD_MID) && !(ep&EQP_HEAD_TOP) && !(ep&EQP_GARMENT) )
		return 0;

	log_pick_pc(sd, LOG_TYPE_SCRIPT, -1, &sd->status.inventory[i]);
	pc_unequipitem(sd,i,2);
	clif_delitem(sd,i,1,3);
	// --------------------------------------------------------------------
	sd->status.inventory[i].refine = 0;
	sd->status.inventory[i].card[0] = CARD0_CREATE;
	sd->status.inventory[i].card[1] = 0;
	sd->status.inventory[i].card[2] = GetWord(battle_config.costume_reserved_char_id, 0);
	sd->status.inventory[i].card[3] = GetWord(battle_config.costume_reserved_char_id, 1);

	if( ep&EQP_HEAD_TOP ) { ep &= ~EQP_HEAD_TOP; ep |= EQP_COSTUME_HEAD_TOP; }
	if( ep&EQP_HEAD_LOW ) { ep &= ~EQP_HEAD_LOW; ep |= EQP_COSTUME_HEAD_LOW; }
	if( ep&EQP_HEAD_MID ) { ep &= ~EQP_HEAD_MID; ep |= EQP_COSTUME_HEAD_MID; }
	if( ep&EQP_GARMENT ) { ep &= ~EQP_GARMENT; ep |= EQP_COSTUME_GARMENT; }
	// --------------------------------------------------------------------
	log_pick_pc(sd, LOG_TYPE_SCRIPT, 1, &sd->status.inventory[i]);

	clif_additem(sd,i,1,0);
	pc_equipitem(sd,i,ep);
	clif_misceffect(&sd->bl,3);

	return 0;
}

BUILDIN_FUNC(playertalk)
{
	int display_name;
	const char* message;
	char message_output[255];
	struct map_session_data *sd = script_rid2sd(st);
  
	display_name = script_getnum(st, 2);
	message = script_getstr(st, 3);
  
	if(display_name)
		sprintf(message_output, "%s: %s", sd->status.name, message);
	else
		sprintf(message_output, "%s", message);
  
	clif_disp_overhead(&sd->bl, message_output);

	return 0;
}

BUILDIN_FUNC(flooritem)
{
	struct map_session_data *sd = script_rid2sd(st);
	struct item_data *item_data;
	int nameid, amount;

	if( sd == NULL ) return 0;

	nameid = script_getnum(st,2);
	if( (item_data = itemdb_search(nameid)) == NULL )
		return 0;

	amount = script_getnum(st,3);
	if( amount <= 0 )
		return 0;

	map_addflooritem_area(&sd->bl, 0, 0, 0, nameid, amount);
	return 0;
}
 /*==========================================
 * Mado Heal with Potions
 *------------------------------------------*/
BUILDIN_FUNC(madoheal)
{
	TBL_PC *sd;
	int hp;

	hp=script_getnum(st,2);

	if(potion_flag==1) {
		potion_hp = hp;
		return 0;
	}
	sd = script_rid2sd(st);
	if (!sd) return 0;
	if( !pc_ismadogear(sd) ) return 0;
	pc_itemheal(sd,sd->itemid,hp,0);
	return 0;
}
