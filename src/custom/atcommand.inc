// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, 343, AREA);
//	return 0;
//}
/**
 * Adopt a character.
 * Usage: @adopt <char name>
 * https://rathena.org/board/topic/104014-suggestion-add-adopt-or-etc/
 */
ACMD_FUNC(adopt)
{
	TBL_PC *b_sd;
	enum adopt_responses response;

	nullpo_retr(-1, sd);

	memset(atcmd_output, '\0', sizeof(atcmd_output));
	memset(atcmd_player_name, '\0', sizeof(atcmd_player_name));

	if (!message || !*message || sscanf(message, "%23[^\n]", atcmd_player_name) < 1) {
		sprintf(atcmd_output, msg_txt(sd, 435), command); // Please enter a player name (usage: %s <char name>).
		clif_displaymessage(fd, atcmd_output);
		return -1;
	}

	if ((b_sd = map_nick2sd((char *)atcmd_player_name)) == NULL) {
		clif_displaymessage(fd, msg_txt(sd, 3)); // Character not found.
		return -1;
	}

	response = pc_try_adopt(sd, map_charid2sd(sd->status.partner_id), b_sd);

	if (response == ADOPT_ALLOWED) {
		TBL_PC *p_sd = map_charid2sd(sd->status.partner_id);

		b_sd->adopt_invite = sd->status.account_id;
		clif_Adopt_request(b_sd, sd, p_sd->status.account_id);
		return 0;
	}
	if (response < ADOPT_MORE_CHILDREN) { // No displaymessage for client-type responses
		short msg_response = (744 + response - 1);
		sprintf(atcmd_output, msg_txt(sd, msg_response));
		clif_displaymessage(fd, atcmd_output);
	}
	return -1;
}

 /*==========================================
 * Battleground Leader Commands
 *------------------------------------------*/
ACMD_FUNC(order)
{
	nullpo_retr(-1,sd);
	if( !message || !*message )
	{
		clif_displaymessage(fd, "Please, enter a message (usage: @order <message>).");
		return -1;
	}

	if( map[sd->bl.m].flag.battleground )
	{
		if( !sd->bmaster_flag )
		{
			clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
			return -1;
		}
		clif_broadcast2(&sd->bl, message, (int)strlen(message)+1, sd->bmaster_flag->color, 0x190, 20, 0, 0, BG);
	}
	else
	{
		if( !sd->state.gmaster_flag )
		{
			clif_displaymessage(fd, "This command is reserved for Guild Leaders Only.");
			return -1;
		}
		clif_broadcast2(&sd->bl, message, (int)strlen(message)+1, 0xFF0000, 0x190, 20, 0, 0, GUILD);
	}

	return 0;
}

ACMD_FUNC(leader)
{
	struct map_session_data *pl_sd;
	nullpo_retr(-1,sd);
	if( !sd->bmaster_flag )
		clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
	else if( sd->ud.skilltimer != INVALID_TIMER )
		clif_displaymessage(fd, "Command not allow while casting a skill.");
	else if( !message || !*message )
		clif_displaymessage(fd, "Please, enter the new Leader name (usage: @leader <name>).");
	else if( (pl_sd = map_nick2sd((char *)message)) == NULL )
		clif_displaymessage(fd, msg_txt(sd,3)); // Character not found.
	else if( sd->bg_id != pl_sd->bg_id )
		clif_displaymessage(fd, "Destination Player is not in your Team.");
	else if( sd == pl_sd )
		clif_displaymessage(fd, "You are already the Team Leader.");
	else
	{ // Everytest OK!
		sprintf(atcmd_output, "Team Leader transfered to [%s]", pl_sd->status.name);
		clif_broadcast2(&sd->bl, atcmd_output, (int)strlen(atcmd_output)+1, sd->bmaster_flag->color, 0x190, 20, 0, 0, BG);

		sd->bmaster_flag->leader_char_id = pl_sd->status.char_id;
		pl_sd->bmaster_flag = sd->bmaster_flag;
		sd->bmaster_flag = NULL;

		clif_charnameupdate(sd);
		clif_charnameupdate(pl_sd);
		return 0;
	}
	return -1;
}

ACMD_FUNC(reportafk)
{
	struct map_session_data *pl_sd;
	nullpo_retr(-1,sd);
	if( !sd->bg_id )
		clif_displaymessage(fd, "This command is reserved for Battleground Only.");
	else if( !sd->bmaster_flag && battle_config.bg_reportafk_leaderonly )
		clif_displaymessage(fd, "This command is reserved for Team Leaders Only.");
	else if( !message || !*message )
		clif_displaymessage(fd, "Please, enter the character name (usage: @reportafk <name>).");
	else if( (pl_sd = map_nick2sd((char *)message)) == NULL )
		clif_displaymessage(fd, msg_txt(sd,3)); // Character not found.
	else if( sd->bg_id != pl_sd->bg_id )
		clif_displaymessage(fd, "Destination Player is not in your Team.");
	else if( sd == pl_sd )
		clif_displaymessage(fd, "You cannot kick yourself.");
	else if( pl_sd->state.bg_afk == 0 )
		clif_displaymessage(fd, "The player is not AFK on this Battleground.");
	else
	{ // Everytest OK!
		struct battleground_data *bg;
		if( (bg = bg_team_search(sd->bg_id)) == NULL )
			return -1;

		bg_team_leave(pl_sd,2);
		clif_displaymessage(pl_sd->fd, "You have been kicked from Battleground because of your AFK status.");
		pc_setpos(pl_sd,pl_sd->status.save_point.map,pl_sd->status.save_point.x,pl_sd->status.save_point.y,3);

		sprintf(atcmd_output, "- AFK [%s] Kicked -", pl_sd->status.name);
		clif_broadcast2(&sd->bl, atcmd_output, (int)strlen(atcmd_output)+1, bg->color, 0x190, 20, 0, 0, BG);
		return 0;
	}
	return -1;
}
/*==========================================
 * Achievements
 *------------------------------------------*/
ACMD_FUNC(achievements)
{
	int i, j;
	struct achievement_data *ad;
	struct s_achievement *sad;
	int x, y;
	char rate[32];

	if( sd->achievement_count <= 0 )
	{
		clif_displaymessage(fd,"You have no progress on any Achievement.");
		return 0;
	}

	for( i = 0; i < sd->achievement_count; i++ )
	{
		sad = &sd->achievement[i];
		if( !sad->id || (ad = achievement_search(sad->id)) == NULL )
			continue;

		if( sad->completed )
			safestrncpy(rate,"100.00%",sizeof(rate));
		else
		{
			x = 0;
			y = 0;

			for( j = 0; j < ad->objectives; j++ )
			{
				x += ad->ao[j].count;
				y += sad->count[j];
			}
			if( x <= 0 )
				safestrncpy(rate,"0.00%",sizeof(rate));
			else
				sprintf(rate,"%02.02f%%", (float)y * 100. / (float)x);
		}

		sprintf(atcmd_output,"[%d] %s. %s Completed.", ad->id, ad->name, rate);
		clif_displaymessage(fd,atcmd_output);
	}

	return 0;
}

ACMD_FUNC(achieve)
{
	int i, id = atoi(message);
	struct achievement_data *ad;

	if( id <= 0 || (ad = achievement_search(id)) == NULL || (i = achievement_index(sd,id)) < 0 )
	{
		clif_displaymessage(fd,"Achievement ID not found.");
		return -1;
	}

	if( sd->achievement[i].completed )
	{
		clif_displaymessage(fd,"Achievement already completed.");
		return -1;
	}

	achievement_complete(sd,ad);
	return 0;
}

ACMD_FUNC(unachieve)
{
	int i, id = atoi(message);
	struct achievement_data *ad;

	if( id <= 0 || (ad = achievement_search(id)) == NULL )
	{
		clif_displaymessage(fd,"Achievement ID not found.");
		return -1;
	}

	ARR_FIND(0,sd->achievement_count,i,sd->achievement[i].id == id);
	if( i < sd->achievement_count )
	{
		if( sd->achievement_count-- < ACHIEVEMENT_MAX && sd->achievement[i+1].id )
			memmove(&sd->achievement[i], &sd->achievement[i+1], sizeof(struct s_achievement)*(sd->achievement_count-i));

		memset(&sd->achievement[sd->achievement_count],0,sizeof(struct s_achievement));
		sd->save_achievement = true;
		clif_displaymessage(fd,"Achievement removed at all from this Character.");
	}
	else
		clif_displaymessage(fd,"Achievement not available for this Character.");

	return 0;
}

ACMD_FUNC(reloadachievements)
{
	achievement_db_load(true);
	clif_displaymessage(fd,"Achievement Database loaded.");
	return 0;
}

ACMD_FUNC(mobevent)
{
	char name[NAME_LENGTH];
	int mob_id,range, count, i, k, number = 1, tamano, hpmas, slaves, spawntipo, showkill, mostrarhp, pasawarps, aliado, hprateannounce, noexpnodrop = 1, item_drop, item_amount;
	short mx, my;
	struct mob_data *md;

	nullpo_retr(-1, sd);

	if( !message || !*message || (sscanf(message, "%23s %d %d %d %d %d %d %d %d %d %d %d %d %d", name, &number, &hpmas, &tamano, &aliado, &spawntipo, &slaves, &pasawarps, &mostrarhp, &hprateannounce, &showkill, &noexpnodrop, &item_drop, &item_amount) < 1) )
	{ // Modo de Uso
		clif_displaymessage(fd, "================================================================================");
		clif_displaymessage(fd, "@mobevent <name/id> <amount> <HP> ...");
		clif_displaymessage(fd, "... <size> 0 normal | 1 small | 2 big");
		clif_displaymessage(fd, "... <type> | 0 : Normal Mob aggresive status.");
		clif_displaymessage(fd, ".......... | 1 : PC Allied / Enemy vs Mob Type 0 and 2.");
		clif_displaymessage(fd, ".......... | 2 : PC and MOB Type 1 Enemy (Aggresive).");
		clif_displaymessage(fd, ".......... | 3 : Ignores PC and PC cannot attack it / Aggresive vs MOD type 1.");
		clif_displaymessage(fd, "... <spawn position> 0 random | 1 your current pos");
		clif_displaymessage(fd, "... <Slaves?> 0 Allow to summon it if the mob can | 1 no slaves allowed");
		clif_displaymessage(fd, "... <warps> 0 cannot use warps | 1 can use map warps | 2 and warp skill too");
		clif_displaymessage(fd, "... <showHP?> 0 Normal | 1 HpMeter Bar | 2 Current/MaxHP | 3 Percent");
		clif_displaymessage(fd, "... <HPrateAnnounce?> 0 off | 1 each 10% less or more HP a map announce appears");
		clif_displaymessage(fd, "... <AnnounceKiller?> 0 off | 1 Map announce of the killer of the mob");
		clif_displaymessage(fd, "... <NoExpnoDrop?> 0 off | 1 on (No exp and job awards)");
		clif_displaymessage(fd, "... <Item ID> Item to be droped when killed (Use -1 to do Item rain event)");
		clif_displaymessage(fd, "... <Item Amount> Amount of previous Item to be droped when killed");
		clif_displaymessage(fd, "================================================================================");
		return -1;
	}

	if( (mob_id = atoi(name)) == 0 )
		mob_id = mobdb_searchname(name);

	if( mob_id == 0 || mobdb_checkid(mob_id) == 0 )
	{
		clif_displaymessage(fd, msg_txt(sd,40)); // Invalid monster ID or name.
		return -1;
	}

	if( battle_config.atc_spawn_quantity_limit >= 1 && number > battle_config.atc_spawn_quantity_limit )
		number = battle_config.atc_spawn_quantity_limit;

	hpmas = cap_value(hpmas, 0, 10000000);
	tamano = cap_value(tamano, 0, 2);
	aliado = cap_value(aliado, 0, 3);
	spawntipo = cap_value(spawntipo, 0, 1);
	slaves = cap_value(slaves, 0, 1);
	pasawarps = cap_value(pasawarps, 0, 2);

	mostrarhp = cap_value(mostrarhp, 0, 3);
	hprateannounce = cap_value(hprateannounce, 0, 1);
	showkill = cap_value(showkill, 0, 1);
	noexpnodrop = cap_value(noexpnodrop, 0, 1);

	// Aditional Drop Event
	if( item_drop > 0 )
	{
		if( !itemdb_exists(item_drop) )
		{
			clif_displaymessage(fd, msg_txt(sd,19)); // Invalid item ID or name.
			return -1;
		}
		else
			item_amount = cap_value(item_amount, 1, 200);
	}
	else
		item_amount = 0;


	count = 0;
	range = (int)sqrt((float)number) +2;
	range = range * 2 + 5; // calculation of an odd number (+ 4 area around)
	if( spawntipo )
	{
		mx = sd->bl.x;
		my = sd->bl.y;
	}

	for( i = 0; i < number; i++ )
	{
		if( !spawntipo )
			map_search_freecell(&sd->bl, 0, &mx, &my, range, range, 0);

		k = mob_once_spawn_especial(sd, "this", mx, my, "--ja--", mob_id, 1, "", hpmas, tamano, aliado, slaves, pasawarps, mostrarhp, hprateannounce, showkill, noexpnodrop, 0, item_drop, item_amount, false, 0, 0);

		if( (md = (struct mob_data *)map_id2bl(k)) )
			count ++;
	}

	if( count != 0 )
		if( number == count )
			clif_displaymessage(fd, msg_txt(sd,39)); // All monster summoned!
		else
		{
			sprintf(atcmd_output, msg_txt(sd,240), count); // %d monster(s) summoned!
			clif_displaymessage(fd, atcmd_output);
		}
	else
	{
		clif_displaymessage(fd, msg_txt(sd,40)); // Invalid monster ID or name.
		return -1;
	}

	return 0;
}
/*=========================================
 * Item Security System
 *-----------------------------------------*/
ACMD_FUNC(security)
{
	nullpo_retr(-1,sd);
	if( sd->npc_id || sd->vender_id || sd->buyer_id || sd->state.trading || sd->state.storage_flag )
		return -1;

	npc_event(sd,"SecuritySystem::OnSettings",0);
	return 0;
}
ACMD_FUNC(expinfo)
{
	unsigned int nextb, nextj, bexp_ps, jexp_ps, nextbt, nextjt;
	int session_time, day = 0, hour = 0, minute = 0, second = 0;

	nullpo_retr(-1,sd);

	if( !strcmpi(message, "reset") )
	{ // Restart Vars
		sd->custom_data.session_base_exp = 0;
		sd->custom_data.session_job_exp = 0;
		sd->custom_data.session_start = last_tick;
	}
	else if( !strcmpi(message, "off") )
	{
		sd->state.showgain = 0;
		clif_displaymessage(fd, msg_txt(sd,864));

		return 0;
	}
	else if( !strcmpi(message, "on") )
	{
		sd->state.showgain = 1;
		clif_displaymessage(fd, msg_txt(sd,865));

		return 0;
	}
	else
		clif_displaymessage(fd, msg_txt(sd,866));

	session_time = DIFF_TICK(last_tick, sd->custom_data.session_start);

	atcommand_expinfo_sub(session_time, &day, &hour, &minute, &second);
	sprintf(atcmd_output, msg_txt(sd,867), day, hour, minute, second);
	clif_disp_onlyself(sd, atcmd_output, strlen(atcmd_output));

	if( session_time <= 0 )
		return 0;

	nextb = pc_nextbaseexp(sd);
	nextj = pc_nextjobexp(sd);

	sprintf(atcmd_output, msg_txt(sd,868), sd->custom_data.session_base_exp, sd->custom_data.session_job_exp);
	clif_disp_onlyself(sd, atcmd_output, strlen(atcmd_output));

	if( nextb )
	{ // Next Base lvl information
		if( (bexp_ps = sd->custom_data.session_base_exp / session_time) < 1 )
			sprintf(atcmd_output, msg_txt(sd,869), bexp_ps);
		else if( (nextbt = (nextb - sd->status.base_exp) / bexp_ps) < 604800 )
		{
			atcommand_expinfo_sub(nextbt, &day, &hour, &minute, &second);
			if( day )
				sprintf(atcmd_output, msg_txt(sd,870), bexp_ps, day, hour, minute, second);
			else
				sprintf(atcmd_output, msg_txt(sd,871), bexp_ps, hour, minute, second);
		}
		else
			sprintf(atcmd_output, msg_txt(sd,869), bexp_ps);

		clif_disp_onlyself(sd, atcmd_output, strlen(atcmd_output));
	}

	if( nextj )
	{ // Next Job lvl information
		if( (jexp_ps = sd->custom_data.session_job_exp / session_time) < 1 )
			sprintf(atcmd_output, msg_txt(sd,872), jexp_ps);
		else if( (nextjt = (nextj - sd->status.job_exp) / jexp_ps) < 604800 )
		{
			atcommand_expinfo_sub(nextjt, &day, &hour, &minute, &second);
			if( day )
				sprintf(atcmd_output, msg_txt(sd,873), jexp_ps, day, hour, minute, second);
			else
				sprintf(atcmd_output, msg_txt(sd,874), jexp_ps, hour, minute, second);
		}
		else
			sprintf(atcmd_output, msg_txt(sd,872), jexp_ps);

		clif_disp_onlyself(sd, atcmd_output, strlen(atcmd_output));
	}

	return 0;
}
/*==========================================
 * Hunting Mission [Zephyrus]
 *------------------------------------------*/
ACMD_FUNC(mission)
{
	int i, c = 0;
	nullpo_retr(-1,sd);

	if( sd->hunting[0].mob_id == 0 )
	{
		clif_displaymessage(fd, "You are not hunting.");
		return 0;
	}

	for( i = 0; i < 5; i++ )
	{
		if( sd->hunting[i].count < 1 )
			continue;

		sprintf(atcmd_output, "- Hunting %d '%s' (ID : %d)", sd->hunting[i].count, mob_db(sd->hunting[i].mob_id)->jname, sd->hunting[i].mob_id);
		clif_displaymessage(fd, atcmd_output);

		c = sd->hunting[i].count;
	}

	if( c < 1 )
		clif_displaymessage(fd, "Hunting Mission Completed");
	else if( sd->hunting_time < (int)time(NULL) )
		clif_displaymessage(fd, "Hunting Mission Failed");
	else
	{
		int tick = sd->hunting_time - (int)time(NULL),
			day, hour, minute, second;

		atcommand_expinfo_sub(tick, &day, &hour, &minute, &second);
		sprintf(atcmd_output, "Time Left : %02d:%02d:%02d", hour, minute, second);
		clif_displaymessage(fd, atcmd_output);
	}

	return 0;
}
/*==========================================
 * Creación de items en el suelo, en un Area.
 *------------------------------------------*/
ACMD_FUNC(flooritem)
{
	char item_name[100];
	int number = 0, count;
	struct item_data *item_data;
	nullpo_retr(-1, sd);

	memset(item_name, '\0', sizeof(item_name));

	if( !message || !*message || (
		sscanf(message, "\"%99[^\"]\" %d", item_name, &number) < 1 &&
		sscanf(message, "%99s %d", item_name, &number) < 1
	)) {
		clif_displaymessage(fd, "Usage: @flooritem <itemname/itemid> [quantity]");
		return -1;
	}

	if( number <= 0 )
		number = 1;

	if ((item_data = itemdb_searchname(item_name)) == NULL &&
	    (item_data = itemdb_exists(atoi(item_name))) == NULL)
	{
		clif_displaymessage(fd, msg_txt(sd,19)); // Invalid item ID or name.
		return -1;
	}

	count = map_addflooritem_area(&sd->bl, 0, 0, 0, item_data->nameid, number);

	if( count != 0 )
		if( number == count )
			clif_displaymessage(fd, "All items created.");
		else
		{
			sprintf(atcmd_output, "%d item(s) created.", count);
			clif_displaymessage(fd, atcmd_output);
		}
	else {
		clif_displaymessage(fd, "No items created!!.");
		return -1;
	}

	return 0;
}

