SELECT name 
FROM Pokemon AS p,Evolution AS e
WHERE p.id = e.before_id and type = 'grass';