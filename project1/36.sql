SELECT name
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id
WHERE pid IN (SELECT after_id
              FROM Evolution
              WHERE after_id NOT IN (SELECT before_id FROM Evolution))
GROUP BY name
ORDER BY name; 