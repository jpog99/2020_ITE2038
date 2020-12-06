SELECT name,p.id
FROM CatchedPokemon INNER JOIN Pokemon AS p ON pid=p.id
WHERE owner_id IN (SELECT id FROM Trainer WHERE hometown='Sangnok City')
ORDER BY p.id;
