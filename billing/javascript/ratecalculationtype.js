// defines the text that should be displayed inside an html element
function writeLayer(layerID, txt)
{
	if (document.getElementById)
	{
		document.getElementById(layerID).innerHTML = txt;
	}
	else if (document.all)
	{
		document.all[layerID].innerHTML = txt;
	}
	else if (document.layers)
	{
		with (document.layers[layerID].document)
		{
			open();
			write(txt);
			close();
		}
	}
}
