<?PHP

function pimPinboard()
{
  atkimport("atk.layout");
  $layout = &layout::getInstance();

  $user = getUser();
  $pinboardnode = getNode("pinboard.pinboard");
  $nrows = $pinboardnode->selectDb("pinboard_pinboard.showonpim=1 AND pinboard_pinboard.startdate<=now() AND pinboard_pinboard.enddate>=now()");
  if(count($nrows)==0) return "";
  $res= $layout->ret_table_simple(0,true);
  $res.="<tr>";
  
  if(count($nrows)>0)
  {
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      if($i%2==0 && $i!=0) $res.="</tr><tr>";
      $res.='<td>'.display_pinboard($nrows[$i]).'</td>';
    }
    if($i%2!=0) $res.=$layout->ret_td("&nbsp;");
  }
  else
  {
    $res.="<td>".text("no_pinboard_items")."</td>"; 
  }
  $res.="</tr>";
  $res.="</table>";
  $res.='<br>'.href("dispatch.php?atknodetype=pinboard.pinboard&atkaction=admin",text('pim_gotopinboard'),SESSION_NESTED);

  return $res; 
}

function pinDate($date)
{
  return $date["day"]."/".$date["month"]."/".$date["year"]; 
}

function display_pinboard($pinboard)
{
  atkimport("atk.layout");
  $layout = &layout::getInstance();

  $ret.=$layout->data_top();
  $ret.='<tr>';  
  $ret.=$layout->ret_td_datatitle(href("dispatch.php?atknodetype=pinboard.pinboard&atkselector=pinboard_pinboard.id%3D%27".$pinboard["id"]."%27&atkaction=view",$pinboard["subject"],SESSION_NESTED)."<br>
                 ".text("written_by").": ".$pinboard["userid"]["lastname"].",".$pinboard["userid"]["firstname"]." ".text("on")." ".pinDate($pinboard["created_on"]));
  $ret.='</tr><tr class="row1">';
  $ret.=$layout->ret_td($pinboard["description"]);
  $ret.='</tr>';
  $ret.=$layout->data_bottom();
  return $ret;

}

?>