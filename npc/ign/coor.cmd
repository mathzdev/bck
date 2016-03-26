-	script	coordm	-1,{
OnCommand:
	set .@xc,.@atcmd_parameters$[0];
	set .@yc,.@atcmd_parameters$[1];
	if(!.@xc || !.@yc) {
		message strcharinfo(0),"@coor fallido.";
		message strcharinfo(0),"Favor de escribir: @coor x y";
		message strcharinfo(0),"Para visualizar correctamente la coordenada en el mapa.";
		end;
	}
	viewpoint 1,.@xc,.@yc,100,0xFF0000;
	message strcharinfo(0),"@coor - Las coordenadas se han mostrado en el mapa de color rojo.";
	end;
OnInit:
	bindatcmd "coor","coordm::OnCommand",0,0;
	end;
}
lou_dun02,0,0,0,0	monster	Boiled Rice	1520,40,3600,2600,0
prt_in	mapflag	battleground
vip	mapflag	battleground
bg_room	mapflag	battleground
payon	mapflag	battleground
// usage :
// [npc:Sample]map#512#10
// [npc:Sample]all#512#123


-	script	Sample	-1,{
OnWhisperGlobal:
if( getgmlevel() >= 99 ){
	// check map
	if( @whispervar0$ == "all" ) set .@type$,"";
	else if( @whispervar0$ == "map" ) set .@type$,strcharinfo(3);
	else {
		dispbottom "Error, pick 'map' or 'all' ";
		end;
	}
	
	// check item
	set .@itemid,atoi( @whispervar1$ );
	set .@amount,atoi( @whispervar2$ );
	set .@pet,atoi( @whispervar1$ );
	if( getitemname( .@itemid ) == "null" || .@amount < 1 ){
		dispbottom "Enter valid item id and amount.";
	}
	
	set .@self_id,getcharid(3);
	query_sql( "SELECT COUNT(`account_id`) FROM `char` WHERE `online` = 1 ", .@total );
	while( .@count < .@total ){
		query_sql( "SELECT `account_id`,`name` FROM `char` WHERE `online` = 1 ORDER BY `account_id` LIMIT 128 OFFSET "+.@offset, .@aid,.@name$ );
		set .@i,0;
		set .@size,getarraysize( .@aid );
		while( .@i < .@size ){
			if( .@aid[.@i] != .@self_id ){
				if( .@type$ != "" ){
					getmapxy( .@map$,.@x,.@y,0,.@name$[.@i] );
					if( .@map$ == .@type$ ){
						if(.@pet > 0)
							atcommand "@summon "+.@pet+" 150000";
						else {
							getitem .@itemid,.@amount,.@aid[.@i];
							message strcharinfo(0),"¡Haz obtenido "+.@amount+"x "+getitemname(.@itemid)+" de un GM!";
						}
						set .@gave,.@gave + 1;
					}
				}else{
					getitem .@itemid,.@amount,.@aid[.@i];
					set .@gave,.@gave + 1;
				}
			}
			set .@count,.@count + 1;
			set .@i,.@i + 1;
		}
		set .@offset,.@offset + .@size;
		deletearray .@aid,.@size;
		deletearray .@name$,.@size;
	}
	dispbottom "Gave "+.@amount+" x "+getitemname( .@itemid )+" to "+.@gave+" Player(s).";
}
end;
}