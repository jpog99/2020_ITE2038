SELECT name , count(pid)
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id
WHERE owner_id IN (SELECT leader_id FROM Gym)
GROUP BY name ORDER BY name;