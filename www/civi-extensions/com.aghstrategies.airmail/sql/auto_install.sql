SELECT @invalidID := id FROM civicrm_mailing_bounce_type where name = 'Invalid';
SELECT @spamID := id FROM civicrm_mailing_bounce_type where name = 'Spam';
INSERT INTO civicrm_mailing_bounce_pattern (bounce_type_id, pattern) VALUES (@invalidID, 'Dropped: Bounced Address'), (@invalidID, 'Dropped: Invalid'), (@spamID, 'Dropped: Spam Reporting Address');