SELECT AVG(level)
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id 
WHERE owner_id IN (SELECT leader_id FROM Gym)
GROUP BY owner_id;