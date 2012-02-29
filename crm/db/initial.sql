
INSERT INTO `crm_activity_status` VALUES (1, 'Active');
INSERT INTO `crm_activity_status` VALUES (2, 'Done');


INSERT INTO `crm_activity_type` VALUES (1, 'Call');
INSERT INTO `crm_activity_type` VALUES (2, 'Email');
INSERT INTO `crm_activity_type` VALUES (3, 'Meeting');
INSERT INTO `crm_activity_type` VALUES (4, 'Task');

INSERT INTO `crm_campaign_status` VALUES (1, 'New');
INSERT INTO `crm_campaign_status` VALUES (2, 'Planning');
INSERT INTO `crm_campaign_status` VALUES (3, 'Active');
INSERT INTO `crm_campaign_status` VALUES (4, 'Closed');

INSERT INTO `crm_campaign_type` VALUES (1, 'Email');
INSERT INTO `crm_campaign_type` VALUES (2, 'Event');
INSERT INTO `crm_campaign_type` VALUES (3, 'Magazine');
INSERT INTO `crm_campaign_type` VALUES (4, 'Television');
INSERT INTO `crm_campaign_type` VALUES (5, 'Other');

INSERT INTO `crm_account_industry` VALUES (1, 'Telecommunications');
INSERT INTO `crm_account_industry` VALUES (2, 'Education');
INSERT INTO `crm_account_industry` VALUES (3, 'Finance');

INSERT INTO `crm_account_source` VALUES (1, 'Advertisement');
INSERT INTO `crm_account_source` VALUES (2, 'Direct mail');
INSERT INTO `crm_account_source` VALUES (3, 'Radio');
INSERT INTO `crm_account_source` VALUES (4, 'Other');
INSERT INTO `crm_account_source` VALUES (5, 'Television');

INSERT INTO `crm_account_status` VALUES (1, 'Lead');
INSERT INTO `crm_account_status` VALUES (2, 'Prospect');
INSERT INTO `crm_account_status` VALUES (3, 'Qualified');
INSERT INTO `crm_account_status` VALUES (4, 'Developed');
INSERT INTO `crm_account_status` VALUES (5, 'Closed');

INSERT INTO `crm_lead_source` VALUES (1, 'Public relations');
INSERT INTO `crm_lead_source` VALUES (2, 'Cold call');
INSERT INTO `crm_lead_source` VALUES (3, 'Existing customer');
INSERT INTO `crm_lead_source` VALUES (4, 'Employee');
INSERT INTO `crm_lead_source` VALUES (5, 'Partner');
INSERT INTO `crm_lead_source` VALUES (6, 'Conference');

INSERT INTO `crm_lead_status` VALUES (1, 'Closed won');
INSERT INTO `crm_lead_status` VALUES (2, 'Closed lost');
INSERT INTO `crm_lead_status` VALUES (3, 'Negotiation/Review');
INSERT INTO `crm_lead_status` VALUES (4, 'Prospecting');
INSERT INTO `crm_lead_status` VALUES (5, 'New');

INSERT INTO `crm_opportunity_activity_status` VALUES (1, 'Active');
INSERT INTO `crm_opportunity_activity_status` VALUES (2, 'Done');

INSERT INTO `crm_opportunity_status` VALUES (1, 'New');
INSERT INTO `crm_opportunity_status` VALUES (2, 'Closed won');
INSERT INTO `crm_opportunity_status` VALUES (3, 'Closed lost');
INSERT INTO `crm_opportunity_status` VALUES (4, 'Prospecting');
INSERT INTO `crm_opportunity_status` VALUES (5, 'Negotiation/Review');

INSERT INTO `crm_relation_type` VALUES (1, 'Acquired company');
INSERT INTO `crm_relation_type` VALUES (2, 'Acquired by company');
INSERT INTO `crm_relation_type` VALUES (3, 'Consultant for company');
INSERT INTO `crm_relation_type` VALUES (4, 'Supplier for company');

INSERT INTO `crm_salutation` VALUES (1, 'Sir');
INSERT INTO `crm_salutation` VALUES (2, 'Madam');

INSERT INTO `db_sequence` VALUES ('crm_salutation', 2);
INSERT INTO `db_sequence` VALUES ('crm_account_industry', 3);
INSERT INTO `db_sequence` VALUES ('crm_activity_type', 4);
INSERT INTO `db_sequence` VALUES ('crm_lead_source', 6);
INSERT INTO `db_sequence` VALUES ('crm_lead_status', 5);
INSERT INTO `db_sequence` VALUES ('crm_campaign_type', 5);
INSERT INTO `db_sequence` VALUES ('crm_campaign_status', 4);
INSERT INTO `db_sequence` VALUES ('crm_opportunity_status', 5);
INSERT INTO `db_sequence` VALUES ('crm_account_status', 5);
INSERT INTO `db_sequence` VALUES ('crm_account_source', 5);
INSERT INTO `db_sequence` VALUES ('crm_activity_status', 2);
INSERT INTO `db_sequence` VALUES ('crm_relation_type', 4);
INSERT INTO `db_sequence` VALUES ('crm_opportunity_activity_status', 2);
