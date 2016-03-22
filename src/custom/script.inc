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

//BUILDIN_FUNC(example)
//{
//	ShowInfo("buildin_example: It works!\n");
//	script_pushint(st,1);
//	return 0;
//}
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
/*==========================================
* playertalk (sends message to surrounding area)
*------------------------------------------*/
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
   
    clif_disp_overhead(sd, message_output);

    return 0;
}
/**
 * Display script message
 * showscript "<message>"{,<GID>};
 **/
BUILDIN_FUNC(showscript) {
	struct block_list *bl = NULL;
	const char *msg = script_getstr(st,2);
	int id = 0;

	if (script_hasdata(st,3)) {
		id = script_getnum(st,3);
		bl = map_id2bl(id);
	}
	else {
		bl = st->rid ? map_id2bl(st->rid) : map_id2bl(st->oid);
	}

	if (!bl) {
		ShowError("buildin_showscript: Script not attached. (id=%, rid=%d, oid=%d)\n", id, st->rid, st->oid);
		script_pushint(st,0);
		return SCRIPT_CMD_FAILURE;
	}

	clif_showscript(bl, msg);

	script_pushint(st,1);
	return SCRIPT_CMD_SUCCESS;
}