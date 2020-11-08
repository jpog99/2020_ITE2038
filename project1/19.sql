SELECT type, count(type)
FROM CatchedPokemon INNER JOIN Pokemon AS p ON pid=p.id
WHERE owner_id IN (SELECT leader_id FROM Gym WHERE city='Sangnok City')
GROUP BY type;