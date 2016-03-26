-	script	resetsc	-1,{
OnCommand:
	sc_end SC_ALL;
	message strcharinfo(0),"@resetsc";
	message strcharinfo(0),"Efectos eliminados satisfactoriamente.";
	end;
OnInit:
	bindatcmd "resetsc","resetsc::OnCommand",10,10;
	end;
}