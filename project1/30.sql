SELECT id,p.name as e1name, e2.name as e2name, e3.name as e3name
FROM Pokemon p, Evolution e,
            (SELECT before_id,after_id,name
             FROM Evolution join pokemon on id = after_id
             WHERE after_id IN (SELECT before_id FROM Evolution)) as e2,
             
             (SELECT before_id,after_id,name
             FROM Evolution join pokemon on after_id = id) as e3
             
WHERE id=e.before_id and e.before_id=e2.before_id and e2.after_id=e3.before_id
ORDER BY id;
