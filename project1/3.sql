SELECT AVG(level)
FROM CatchedPokemon AS cp
WHERE cp.owner_id IN (SELECT id FROM Trainer WHERE hometown='Sangnok City')
      AND cp.pid IN (SELECT id FROM Pokemon WHERE type='electric') ;