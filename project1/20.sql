SELECT name , count(pid)
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id
WHERE owner_id IN (SELECT id FROM Trainer WHERE hometown='Sangnok City')
GROUP BY name ORDER BY count(pid);