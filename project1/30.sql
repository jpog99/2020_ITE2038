SELECT id,name
FROM Pokemon, Evolution 
WHERE id=before_id AND
      id IN (SELECT before_id
             FROM Evolution
             WHERE after_id IN (SELECT before_id FROM Evolution))
ORDER BY id; 