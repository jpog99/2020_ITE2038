SELECT AVG(level)
FROM CatchedPokemon
WHERE owner_id IN (SELECT id FROM Trainer WHERE name='Red');