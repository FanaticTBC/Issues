DELETE FROM command WHERE NAME = 'account mailchange';
INSERT INTO command(`name`,`security`,`help`) VALUES('account mailchange',2,'Syntax: .account mailchange $account $newmail\n\nChanger le mail annule le changement de mail en cours.');