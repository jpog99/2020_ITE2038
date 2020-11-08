SELECT name
FROM POKEMON
WHERE id IN (SELECT after_id
             FROM Evolution
             WHERE after_id NOT IN (SELECT before_id FROM Evolution))
ORDER BY name;
      
