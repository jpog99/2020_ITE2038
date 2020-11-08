SELECT name,level,nickname
FROM CatchedPokemon INNER JOIN Pokemon AS p ON pid = p.id
WHERE owner_id IN (SELECT leader_id FROM Gym) AND nickname LIKE "A%"
ORDER BY name DESC;