function togglePhaseActivities(initiator)
{
  var phaseSelector = initiator.id.indexOf('phase');
  var projectSelector = initiator.id.indexOf('project');
  
  if (projectSelector > 0 && window.open_phaseselector_box != null)
  {
    window.open_phaseselector_box.style.display = 'none';
    
    if (window.open_activityselector_box != null)
      window.open_activityselector_box.style.display = 'none';
  }
    
  if (phaseSelector > 0 && window.open_activityselector_box != null)
    window.open_activityselector_box.style.display = 'none';
  
  var project_selector_element = document.getElementById('weekhours_add_project_selector');
  var project_id = project_selector_element.value;
  
  if (project_id == 0) return;
  
  var phase_selector_element = document.getElementById('weekhours_add_phase_selector_' + project_id);
  var phase_id = phase_selector_element.value;
  
  phase_selector_element.style.display = 'block';
  window.open_phaseselector_box = phase_selector_element;
  
  if (phase_id == 0) return;
  
  var activity_element_id = 'weekhours_add_activity_selector_'+phase_id+'_'+project_id;
  var activity_selector_element = document.getElementById(activity_element_id);
  var activity_id = activity_selector_element.value;
  
  activity_selector_element.style.display = 'block';
  window.open_activityselector_box = activity_selector_element;
  
  if (activity_id > 0)
  {
    var submitButtonElement = document.getElementById('weektimereg_by_team_submitbutton');
    submitButtonElement.style.display = 'block';
  }
  else
  {
    var submitButtonElement = document.getElementById('weektimereg_by_team_submitbutton');
    submitButtonElement.style.display = 'none';
  }
}