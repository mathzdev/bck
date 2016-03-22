-	script	storagelist2	-1,{
	end;
OnCommand: //@storagelist2 command.
	if ( !( .@aid = getcharid( 3, .@atcmd_parameters$ ) ) ) {
		if ( !query_sql( "select account_id from `char` where name = '"+ escape_sql(.@atcmd_parameters$)+"'", .@aid ) ) {
			mes "no such player";
			close;
		}
	}
	query_sql( "select count(1) from storage where account_id = "+ .@aid, .@total );
	while (1) {
		.@nb = query_sql( "select nameid, amount, identify, refine, attribute, card0, card1, card2, card3 from storage where account_id = "+ .@aid +" limit "+ .page +" offset "+ .page * .@current_page, .@itemid, .@amount, .@identify, .@refine, .@broken, .@card1, .@card2, .@card3, .@card4 );
		mes "= Total "+ .@total +" items, Page No."+( .@current_page +1 )+" =";
		for ( .@i = 0; .@i < .@nb; .@i++ )
			mes .@amount[.@i] +"x "+ @itemname2_info$[0] + callfunc( "getitemname2", .@itemid[.@i], .@identify[.@i], .@refine[.@i], .@broken[.@i], .@card1[.@i], .@card2[.@i], .@card3[.@i], .@card4[.@i] ) +"^000000";
		next;
		if ( select ( "Next Page", "Previous Page" ) == 1 ) {
			if ( .page * ( .@current_page +1 ) < .@total )
				.@current_page++;
			else {
				mes "the end of the page";
				close;
			}
		}
		else {
			if ( .@current_page == 0 ) {
				mes "this is the 1st page";
				close;
			}
			else
				.@current_page--;
		}
	}
	close;
OnInit:
	bindatcmd "storagelist2", "storagelist2::OnCommand", 40,40; // 40 for atcommand, 40 for charcommand
	.page = 5; // display 5 items per page
	end;
}