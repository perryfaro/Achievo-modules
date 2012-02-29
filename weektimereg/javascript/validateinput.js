function validateInput(el, text)
{
  totaltime = el.value;
  pos = totaltime.indexOf(":");
  if (pos >= 0)
  {
    time = totaltime.substring(pos+1);
  }
  else
  {
    time = "";
  }
  if (el.value != "" && time != "" && time != "00" && time != "0" && time != "15" && time != "30" && time != "45")
  {
    alert(text);
    el.focus();
  }
}