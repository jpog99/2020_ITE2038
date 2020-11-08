SELECT t.name, AVG(level)
FROM CatchedPokemon AS cp INNER JOIN Trainer AS t ON owner_id = t.id
WHERE owner_id IN (SELECT leader_id FROM Gym) GROUP BY owner_id ORDER BY t.name;