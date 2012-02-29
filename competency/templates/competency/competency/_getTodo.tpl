  <table class=todotable>
  {foreach from=$todos key=domkey item=domTodo}
   <thead>
      <tr>
        <td colspan="3"><span style="font-size:large;font-weight:bold">{$domkey}</span></td>
      </tr>
    </thead>
    {foreach from=$domTodo key=compkey item=compTodo}
      <tr class=domaincell>
        <td><span style="font-weight:bolder">{$compkey}</span></td>
        <td></td>
      </tr>
      {if $comments.$domkey.$compkey.comp_comment != ""}
	  <tr>
        <td colspan=3> <b>comment</b></td>
      </tr>
      <tr>
        <td colspan=3>{$comments.$domkey.$compkey.comp_comment}</td>
      </tr>
	  {/if}
      {if $compTodo|is_array}
        <tr>
          <td></td>
          <td><b>level</b></td>
          <td><b>Plan date</b></td>
        </tr>
        {foreach from=$compTodo key=nivkey item=nivTodo}
          <tr>
            <td></td>
            <td>{$nivkey}</td>
            <td>{foreach from=$nivTodo key=datekey item=dateMessage}{$datekey}<br/>{/foreach}</td>
          </tr>
          <tr>
            <td></td>
            <td colspan="2">{foreach from=$nivTodo key=datekey item=dateMessage}{if $dateMessage}{$dateMessage}{/if}<br />{/foreach}</td>
          </tr>
        {/foreach}
      {/if}
    {/foreach}
  {/foreach}
 </table>
