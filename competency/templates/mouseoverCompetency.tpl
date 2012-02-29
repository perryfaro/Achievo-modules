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
{*we check if competences are planned*}
{if $comps|@count > 0}
   <tr>
     <td  class='nameCol'>
       {atktext id="acquired" module="competency" lng=$lng}:
    </td>
  </tr>
  <tr>
    <td>
      {$comps[0].added}
    </td>
  </tr>
{else}
  {assign var=hadFirst value=false}
  {foreach from=$schedules  item=schedule}
        <tr>
          <td class='nameCol'>
            <b>{atktext id="planned_for" module="competency" lng=$lng}:</b>
          </td>
          <td>
            {$schedule.plandate}
          </td>
        </tr>
        {if !$hadFirst && $schedules|@count>1}
          <tr>
            <td class='nameCol'>----------------------</td>
            <td></td>
          </tr>
         <tr>
           <td class='nameCol'>
             {atktext id="history" module="competency" lng=$lng}:
           </td>
           <td></td>
         </tr>
         {assign var=hadFirst value=true}
        {/if}
     {/foreach}
{/if}
</table>