SELECT name,level
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id
WHERE level IN (
                   SELECT max(level)
                   FROM CatchedPokemon 
                   GROUP BY owner_id HAVING count(pid)>=4 
                  )
ORDER BY name;