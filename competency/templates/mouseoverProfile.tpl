<table class='overTable'>
{if $table == "competency"}
  {foreach from=$competences key=ckey item=competence}
    <tr>
      <td class='nameCol'>
            {atktext id="domain" module="competency" lng=$lng}:
      </td>
      <td>
         {$competence.cDomain}
       </td>
     </tr>
      <tr>
         <td class='nameCol'>
            {atktext id="competence" module="competency" lng=$lng}:
         </td>
          <td>
            {$competence.competency}
          </td>
      </tr>
  {/foreach}
{else}
  {foreach from=$competences key=ckey item=competence}
     <tr>
       <td class='nameCol'></td>
       <td></td>
     </tr>

     <tr>
       <td class='nameCol'>
          {atktext id="domain" module="competency" lng=$lng}:
       </td>
       <td>
         {$competence.cDomain}
       </td>
     </tr>

     <tr>
       <td class='nameCol'>
         {atktext id="competence" module="competency" lng=$lng}:
       </td>
       <td>
          {$competence.competency}
       </td>
     </tr>

     <tr>
       <td class='nameCol'>
         {atktext id="level" module="competency" lng=$lng} {$competence.level}:
       </td>
       <td>
       </td>
     </tr>

     <tr>
       <td colspan=2>
         {$competence.name}
       </td>
     </tr>
  {/foreach}
{/if}
</table>