SELECT name
FROM Trainer
WHERE id IN (
    SELECT owner_id
    FROM CatchedPokemon
    GROUP BY owner_id, pid
    HAVING COUNT(*) >= 2
)
ORDER BY name;